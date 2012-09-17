#!/bin/bash

if [[ $@<3 ]]
	then
	echo "Usage: $0 <xml_dir> <output_dir>";
	exit 1;
fi

# Generates the html documentation out of the xml files
files=(spruce main syscall common benchmark ext4fs)

function copy_htmls()
{
	local xml_dir=$1
	local output_dir=$2
	
	file $xml_dir/index.html > /dev/null 2>&1 || return
	
	echo "Copying files"
	
	cp $xml_dir/*.html $output_dir > /dev/null 2>&1
	
	for file in "${files[@]}"
	do 
		echo "Copying $file";
		mkdir $output_dir/$file/ > /dev/null 2>&1
		cp $xml_dir/$file/*.html $output_dir/$file/ > /dev/null 2>&1
	done
	
	exit 0;
}

copy_htmls $1 $2

xml_dir=$1
output_dir=$2


echo "Storing output in $output_dir"
cp -r $xml_dir/*.xml $output_dir/
cd $output_dir

# First prepare the olink target databases
for file in "${files[@]}"
do 
	echo "Generating target database for file $file";
	xsltproc --stringparam  collect.xref.targets "only" /usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl $file.xml > /dev/null 2>&1 && mv target.db $file.db
done

# Generate the documentation 
for file in "${files[@]}"
do 
	echo "Generating documentation for $file";
	xsltproc --output $file/ /usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl $file.xml  > /dev/null 2>&1
done
mv spruce/*.html .
rm -r spruce/
