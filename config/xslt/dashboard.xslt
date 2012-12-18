<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output
    method="html"    
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    indent="yes"
    encoding="utf-8" />
    
<xsl:key name="DistinctMountOptions" match="/SpruceDashboard/MountOptions/Option" use="." />

<xsl:template match="/SpruceDashboard">
<html>
	<head>
		<style>
			.Success
			{
				color:Green;
			}
			.Shallow
			{
				color:Lime;
			}
			.Unresolved
			{
				color:Orange;
			}
			.Failed
			{
				color:Red;
			}
			.Unsupported
			{
				color:Gray;
			}
			.Timeout
			{
				color:Blue;
			}
		</style>
		<title>Spruce dashboard for filesystem <xsl:value-of select="@FS" /></title>
	</head>
	<body>
		<h1 style="text-align:center">Spruce dashboard for filesystem <b><xsl:value-of select="@FS" /></b></h1>
		<h2 style="text-align:center">Start: <xsl:value-of select="Start" /></h2>
		<h2 style="text-align:center">Duration: <xsl:value-of select="Duration" />s</h2>
		<h2 style="text-align:center">Revision: <xsl:value-of select="Rev" />s</h2>
		<table border="1" cellspacing="0" align="center">
			<tr>
				<th>MountOptions</th>
				<xsl:for-each select="Modules/Module">
					<th><xsl:value-of select="."/></th>
				</xsl:for-each>
			</tr>
			<xsl:for-each select="//MountOptions/Option[generate-id()=generate-id(key('DistinctMountOptions',.)[1])]">
				<xsl:variable name="MountOptions" select="." />
				<tr>
					<td>
						<xsl:value-of select="."/>
					</td>
					<xsl:for-each select="//Modules/Module">
						<td>
							<xsl:variable name="LogFile"><xsl:value-of select="$LogFolder"/>/<xsl:value-of select="/SpruceDashboard/@FS"/>_<xsl:value-of select="."/>_<xsl:value-of select="$MountOptions"/>_log.xml</xsl:variable>
							<xsl:variable name="LogFileHtml"><xsl:value-of select="$LogFolder"/>/<xsl:value-of select="/SpruceDashboard/@FS"/>_<xsl:value-of select="."/>_<xsl:value-of select="$MountOptions"/>_log.html</xsl:variable>
							<xsl:choose>
								<xsl:when test="document($LogFile)">
									<a>
										<xsl:attribute name="href"><xsl:value-of select="$LogFileHtml"/></xsl:attribute>
										<span class="Success">
											<xsl:value-of select="count(document($LogFile)//Item[Status='Success' or Status='Shallow' or Status='FSimFail'])" /> 
										</span> / 
										<span class="Failed">
											<xsl:value-of select="count(document($LogFile)//Item[Status='Fatal' or Status='Failed' or Status='Signaled' or Status='Timeout' or Status='FSimSuccess'])" /> 
										</span> / 
										<span class="Unresolved">
											<xsl:value-of select="count(document($LogFile)//Item[Status='Unresolved' or Status='Unsupported' or Status='Skipped'])" />
										</span>
									</a>
								</xsl:when>
								<xsl:otherwise>
									-
								</xsl:otherwise>
							</xsl:choose>
						</td>
					</xsl:for-each>
				</tr>
			</xsl:for-each>
		</table>		
	</body>
</html>
</xsl:template>

</xsl:stylesheet>
