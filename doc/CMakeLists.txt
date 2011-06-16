cmake_minimum_required(VERSION 2.4)

message("Configuring project documentation.")

project (Spruce)

add_custom_command(	OUTPUT index.html
					DEPENDS spruce.xml main/index.html
					COMMAND xsltproc --output index.html /usr/share/xml/docbook/stylesheet/docbook-xsl/html/docbook.xsl spruce.xml)

add_custom_command(	OUTPUT main/index.html
					DEPENDS main.xml
					COMMAND xsltproc --output main/ /usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl main.xml)

add_custom_target(	docs 
					DEPENDS main.xml)
					

install (FILES ${CMAKE_BINARY_DIR}/doc/index.html DESTINATION share/doc/spruce)
install (FILES main/index.html DESTINATION share/doc/spruce/main)
install (FILES main/ch01.html DESTINATION share/doc/spruce/main)
