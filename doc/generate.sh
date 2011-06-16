#!/bin/bash
output_dir=$1
echo "Storing output in $output_dir"
cd $output_dir

# Generates the html documentation out of the xml files
files=(spruce main syscall common benchmark)

# First prepare the olink target databases
for file in "${files[@]}"
do 
	echo "Generating target database for file $file";
	xsltproc --stringparam  collect.xref.targets "only" --output $output_dir /usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl $file.xml && mv target.db $file.db
done

# Generate the documentation itself
for file in "${files[@]}"
do 
	xsltproc --output $output_dir/$file/ /usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl $file.xml
done
mv $output_dir/spruce/index.html $output_dir/spruce.html
rm -r $output_dir/spruce/
