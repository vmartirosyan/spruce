<?xml version="1.0" encoding="utf-8"?>
<!--    testpackage.xslt
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//      	Ruzanna Karakozova <r.karakozova@gmail.com>
//			Vahram Martirosyan <vmartirosyan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
-->

<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:exsl="http://exslt.org/common"
		extension-element-prefixes="exsl">

<xsl:output method="text" encoding="utf-8" indent="yes"/>
<xsl:include href="testset.xslt" />

	<xsl:template match="/TestPackage">

#include &lt;Common.hpp>
#include &lt;Test.hpp>
#include &lt;File.hpp>
#include &lt;Directory.hpp>
#include &lt;map>
#include &lt;sys/vfs.h>
#include &lt;XmlSpruceLog.hpp>
#include &lt;vector>
using namespace std;
		<xsl:variable name="TestPackageName" select="@Name" />		

		

TestPackage Init<xsl:value-of select="$TestPackageName" />()
{

	<xsl:for-each select="//TestSet">	
			<xsl:variable name="TestClassName"><xsl:value-of select="$TestPackageName" /><xsl:value-of select="@Name" />Tests</xsl:variable>
			<xsl:variable name="TestSetFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.xml</xsl:variable>
			<xsl:variable name="SourceFile"><xsl:value-of select="$XmlFolder"/>/<xsl:value-of select="@Name"/>.cpp</xsl:variable>
			<!--exsl:document href="{$SourceFile}" method="text">
				<xsl:apply-templates select="document($TestSetFile)" >
					<xsl:with-param name="PackageName" select="$TestPackageName"/>
					<xsl:with-param name="TestClassName" select="$TestClassName"/>
				</xsl:apply-templates>
			</exsl:document-->
	TestSet Init_<xsl:value-of select="$TestPackageName" />_<xsl:value-of select="@Name" />();
	
	</xsl:for-each>

	TestPackage <xsl:value-of select="$TestPackageName" />Pack("<xsl:value-of select="$TestPackageName" />");
	
	<xsl:for-each select="//TestSet">
		
	<xsl:value-of select="$TestPackageName" />Pack.AddTestSet(Init_<xsl:value-of select="$TestPackageName" />_<xsl:value-of select="@Name" />());
	
	</xsl:for-each>
	
	
	
	return <xsl:value-of select="$TestPackageName" />Pack;

}
			
	</xsl:template>
</xsl:stylesheet>
