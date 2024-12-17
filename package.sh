# Cleanup old
rm -rf yedit*.gz
rm -rf yedit-1.0-1

# Create debian structure
mkdir -p yedit-1.0-1
cp -r src/* yedit-1.0-1
cp -r ./debian yedit-1.0-1/
cd yedit-1.0-1
# tar czf ./yedit-1.0-1.orig.tar.gz --exclude=debian --exclude=build --exclude=bin --exclude=yedit*.gz --transform 's,^,yedit-1.0/,' .
# cp ./yedit-1.0-1.orig.tar.gz ./yedit-1.0_1.tar.gz

# dpkg-buildpackage -us -uc
debuild -us -uc
cd ..
