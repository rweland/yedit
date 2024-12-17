#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <yaml.h>
#include <json-c/json.h>

void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void copy_file(const char *src, const char *dest)
{
    FILE *src_file = fopen(src, "r");
    if (!src_file)
        die("Error opening source file");

    FILE *dest_file = fopen(dest, "w");
    if (!dest_file)
    {
        fclose(src_file);
        die("Error opening destination file");
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src_file)) > 0)
    {
        if (fwrite(buffer, 1, bytes, dest_file) != bytes)
        {
            fclose(src_file);
            fclose(dest_file);
            die("Error writing to destination file");
        }
    }

    fclose(src_file);
    fclose(dest_file);
}

int files_are_identical(const char *file1, const char *file2)
{
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    if (!f1 || !f2)
        die("Error opening files for comparison");

    char buf1[4096], buf2[4096];
    size_t len1, len2;

    int identical = 1;
    while ((len1 = fread(buf1, 1, sizeof(buf1), f1)) > 0 &&
           (len2 = fread(buf2, 1, sizeof(buf2), f2)) > 0)
    {
        if (len1 != len2 || memcmp(buf1, buf2, len1) != 0)
        {
            identical = 0;
            break;
        }
    }

    fclose(f1);
    fclose(f2);

    return identical;
}

void yaml_to_json(const char *yaml_file, const char *json_file)
{
    FILE *file = fopen(yaml_file, "r");
    if (!file)
        die("Error opening YAML file");

    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser))
        die("Failed to initialize YAML parser");
    yaml_parser_set_input_file(&parser, file);

    struct json_object *json_root = json_object_new_object();
    yaml_event_t event;

    char key[256] = "";
    char value[256] = "";
    int in_key = 1;

    while (yaml_parser_parse(&parser, &event))
    {
        if (event.type == YAML_SCALAR_EVENT)
        {
            const char *data = (const char *)event.data.scalar.value;
            if (in_key)
            {
                strncpy(key, data, sizeof(key) - 1);
                key[sizeof(key) - 1] = '\0';
                in_key = 0;
            }
            else
            {
                strncpy(value, data, sizeof(value) - 1);
                value[sizeof(value) - 1] = '\0';
                json_object_object_add(json_root, key, json_object_new_string(value));
                in_key = 1;
            }
        }
        else if (event.type == YAML_STREAM_END_EVENT)
        {
            break;
        }
        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);

    FILE *json_out = fopen(json_file, "w");
    if (!json_out)
        die("Error opening JSON file");

    fprintf(json_out, "%s", json_object_to_json_string_ext(json_root, JSON_C_TO_STRING_PRETTY));
    fclose(json_out);
    json_object_put(json_root);
}

void json_to_yaml(const char *json_file, const char *yaml_file)
{
    FILE *file = fopen(json_file, "r");
    if (!file)
        die("Error opening JSON file");

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_content = malloc(length + 1);
    if (!json_content)
        die("Error allocating memory for JSON content");

    fread(json_content, 1, length, file);
    json_content[length] = '\0';
    fclose(file);

    struct json_object *json_root = json_tokener_parse(json_content);
    if (!json_root)
        die("Error parsing JSON content");
    free(json_content);

    FILE *yaml_out = fopen(yaml_file, "w");
    if (!yaml_out)
        die("Error opening YAML file");

    struct json_object_iterator it = json_object_iter_begin(json_root);
    struct json_object_iterator it_end = json_object_iter_end(json_root);

    while (!json_object_iter_equal(&it, &it_end))
    {
        const char *key = json_object_iter_peek_name(&it);
        struct json_object *val = json_object_iter_peek_value(&it);

        fprintf(yaml_out, "%s: %s\n", key, json_object_get_string(val));
        json_object_iter_next(&it);
    }

    fclose(yaml_out);
    json_object_put(json_root);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *original_file = argv[1];

    // Ensure the original file is accessible
    if (access(original_file, R_OK | W_OK) != 0)
        die("Cannot access file");

    // Ensure the original file is a YAML file
    if (strstr(original_file, ".yaml") == NULL && strstr(original_file, ".yml") == NULL)
        die("File is not a YAML file");

    // Create a temporary JSON file
    char temp_json_template[] = "/tmp/yedit_XXXXXX";
    int temp_json_fd = mkstemp(temp_json_template);
    if (temp_json_fd == -1)
        die("Error creating temporary JSON file");
    close(temp_json_fd);

    // Convert the original YAML file to JSON and write to the temporary JSON file
    yaml_to_json(original_file, temp_json_template);

    // Make a copy of the JSON file for tracking changes
    char temp_json_backup_template[] = "/tmp/yedit_backup_XXXXXX";
    int temp_json_backup_fd = mkstemp(temp_json_backup_template);
    if (temp_json_backup_fd == -1)
        die("Error creating backup JSON file");
    close(temp_json_backup_fd);
    copy_file(temp_json_template, temp_json_backup_template);

    // Determine the editor to use
    const char *editor = getenv("EDITOR");
    if (!editor)
        editor = "vi";

    // Launch the editor with the temporary JSON file
    char command[512];
    snprintf(command, sizeof(command), "%s %s", editor, temp_json_template);
    int ret = system(command);
    if (ret == -1)
    {
        unlink(temp_json_template);
        unlink(temp_json_backup_template);
        die("Error launching editor");
    }

    // Check if the JSON file was modified
    if (!files_are_identical(temp_json_template, temp_json_backup_template))
    {
        // Reconcile changes back to the original file
        json_to_yaml(temp_json_template, original_file);
        printf("Changes saved to %s\n", original_file);
    }
    else
    {
        printf("No changes made to %s\n", original_file);
    }

    // Clean up temporary files
    unlink(temp_json_template);
    unlink(temp_json_backup_template);

    return EXIT_SUCCESS;
}
