<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output
    method="html"    
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    indent="yes"
    encoding="utf-8" />
    
<xsl:key name="Syscalls" match="//SysCall" use="@Name" />

<xsl:template match="/SpruceLog">
<html>
	<head>
		<style>
			.Success
			{
				color:Green;
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
		<!--xsl:for-each select="//FS"-->
			
			<xsl:variable name="fs" select="//FS/@Name" />
			<h1>Filesystem: <xsl:value-of select="$fs" /></h1>
			<H2>Module: Syscalls</H2>
			
			<xsl:for-each select="//SysCall[count(. | key('Syscalls', @Name)[1]) = 1]">
			
				
				<xsl:sort select="@Name" />
				
				<xsl:variable name="SysCall" select="@Name" />
				<xsl:variable name="TestsTotal" select="count(//SysCall[@Name=$SysCall])" />
				<xsl:variable name="TestsPassed" select="count(//SysCall[@Name=$SysCall and Status='Success'])" />
				<span class="Failed">
					<xsl:if test="$TestsPassed=$TestsTotal">
						<xsl:attribute name="class">					
							Success
						</xsl:attribute>
					</xsl:if>
				<xsl:value-of select="$SysCall" />			
				(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
				</span>
				<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$SysCall" />')</xsl:attribute> +</a>
				<br />
				<div style="border: solid 1px black; padding: 5px; display:none;">	
					<xsl:attribute name="id"><xsl:value-of select="$SysCall" /></xsl:attribute>
					<xsl:for-each select="//SysCall[@Name=$SysCall]">
						<div>
							<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
							<xsl:value-of select="Operation" />: <xsl:value-of select="Status" />
							<xsl:if test="Status!='Success'"><xsl:value-of select="Output" /></xsl:if>
						</div>
					</xsl:for-each>
					<hr />
					Total Tests: <xsl:value-of select="count(//SysCall[@Name=$SysCall])" /><br />
					<span class="Success">Tests passed: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Success'])" /></span> <br/>
					<span class="Failed">Tests failed: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Failed'])" /></span> <br/>
					<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Unresolved'])" /></span> <br/>
					<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//SysCall[@Name=$SysCall and Status='Unsupported'])" /></span> <br/>
				</div>
			</xsl:for-each>	
			
			<hr />
			
			<span class="Success">Total tests passed: <xsl:value-of select="count(//SysCall[Status='Success'])" /></span> <br/>
			<span class="Failed">Total tests failed: <xsl:value-of select="count(//SysCall[Status='Failed'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'TestsFailed')</xsl:attribute> +</a>
			<div id="TestsFailed"  style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
				<xsl:for-each select="//SysCall[Status='Failed']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//SysCall[Status='Unresolved'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'TestsUnresolved')</xsl:attribute> +</a>
			<div id="TestsUnresolved"  style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
				<xsl:for-each select="//SysCall[Status='Unresolved']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
			<span class="Unsupported">Total tests unsupported: <xsl:value-of select="count(//SysCall[Status='Unsupported'])" /></span>
			<a href="#" style="text-decoration:none">
				<xsl:attribute name="onclick">ShowHideTest(this, 'TestsUnsupported')</xsl:attribute> +</a>
			<div id="TestsUnsupported"  style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
				<xsl:for-each select="//SysCall[Status='Unsupported']" >
					<xsl:value-of select="@Name" /> : <xsl:value-of select="Operation" /> : <xsl:value-of select="Output" /><br />
				</xsl:for-each>
			</div>
			<br />
		
		
	</body>
</html>
</xsl:template>

</xsl:stylesheet>
