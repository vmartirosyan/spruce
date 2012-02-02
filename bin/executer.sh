#!/bin/bash

sudo /opt/spruce/bin/syscall > /tmp/spruce_output.xml  &&
xsltproc -o /tmp/spruce_output.html /opt/spruce/config/processor.xslt /tmp/spruce_output.xml &&
sleep 1 &&
which firefox && firefox file:///tmp/spruce_output.html ||
which chrome && chrome file:///tmp/spruce_output.html ||
which opera && opera file:///tmp/spruce_output.html
