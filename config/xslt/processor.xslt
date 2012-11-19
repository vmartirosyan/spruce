<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="module.xslt" />
<xsl:output
    method="html"    
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    indent="yes"
    encoding="utf-8" />


<xsl:template match="/SpruceLog">
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
			.Signalled
			{
				color:DarkRed;
			}
			.Skipped
			{
				color:DarkGrey;
			}
			.Timeout
			{
				color:Blue;
			}
			.Fatal
			{
				color:Purple;
			}
			.Signaled
			{
				color:DarkRed;
			}
			.FSimSuccess
			{
				color:Crimson;
			}
			.FSimFail
			{
				color:Teal
			}
		</style>
		<title>Spruce log for filesystem <xsl:value-of select="//FS/@Name" /></title>
	</head>
	<body>
		<script language="javascript">
			function ShowHideTest(object, ID)
			{					
				document.getElementById(ID).style.display = 
					(document.getElementById(ID).style.display == "none" ? "" : "none");
				object.innerText = (object.innerText == ' -' ? ' +' : ' -');
			}
		</script>
					
		<xsl:variable name="fs" select="//FS/@Name" />
		<h1>Filesystem: <xsl:value-of select="$fs" /></h1>
		<xsl:apply-templates select="//Module" />
		
	</body>
</html>
</xsl:template>

</xsl:stylesheet>
