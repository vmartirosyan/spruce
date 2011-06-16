cmake_minimum_required(VERSION 2.4)

message("Configuring project documentation.")

project (Spruce)

execute_process(COMMAND echo "Generating documentation")

execute_process(COMMAND bash -c "file ${CMAKE_SOURCE_DIR}/doc/index.html > /dev/null 2>&1 && (echo 'Copying documentation' && cp ${CMAKE_SOURCE_DIR}/doc/index.html ${CMAKE_BINARY_DIR}/doc/) || xsltproc --output ${CMAKE_BINARY_DIR}/doc/index.html /usr/share/xml/docbook/stylesheet/docbook-xsl/html/docbook.xsl spruce.xml  > /dev/null 2>&1 "
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)
				
execute_process(COMMAND bash -c "file ${CMAKE_SOURCE_DIR}/doc/main/index.html > /dev/null 2>&1  && (echo 'Copying Main documentation' && mkdir ${CMAKE_BINARY_DIR}/doc/main && cp ${CMAKE_SOURCE_DIR}/doc/main/*.html ${CMAKE_BINARY_DIR}/doc/main) || xsltproc --output ${CMAKE_BINARY_DIR}/doc/main/ /usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl main.xml  > /dev/null 2>&1 "
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)

install (FILES ${CMAKE_BINARY_DIR}/doc/index.html DESTINATION share/doc/spruce)
install (FILES ${CMAKE_BINARY_DIR}/doc/main/index.html ${CMAKE_BINARY_DIR}/doc/main/ch01.html DESTINATION share/doc/spruce/main)

