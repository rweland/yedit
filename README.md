# Yedit  

**Yedit** is a lightweight, intuitive tool for editing YAML files in JSON format. It bridges the gap between two widely-used data formats, making configuration management easier and more accessible.  

## Features  

- **Seamless YAML-to-JSON Editing**: View and edit YAML files directly in JSON format.  
- **Validation**: Ensure your YAML and JSON files are properly structured.  
- **User-Friendly Interface**: Will attempt to use your editor preference.  

## Installation  

### From Source 
Clone the repository:  
```bash  
git clone https://github.com/rweland/yedit.git  
cd yedit  
```
Install dependencies
``` bash
sudo apt install build-essential libyaml-dev libjson-c-dev
```
Build the project
``` bash
make
```
Install to system
``` bash
sudo make install
```

### From Package
WIP

## Usage


## Screenshots
WIP

## Contributing
We welcome contributions!

Fork the repository on GitHub

https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo

Create a feature branch:
```bash
git checkout -b feature-name  
```
Commit your changes:
```bash
git commit -m "Add a meaningful message here"  
```
Push to the branch:
```bash
git push origin feature-name  
```
Open a Pull Request.

https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request


## License
YEdit is licensed under the MIT License.

## Support
If you encounter any issues or have suggestions, please open an issue on GitHub.

## Roadmap
* Bi-directional conversion: Add support for detecting the file being edited and automatically converting it to the other format.
* Cross-platform support: Make YEdit available on Windows and macOS.
* Publishing to package managers: Make YEdit available on package managers like APT and Homebrew.
