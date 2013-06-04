<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="Summary">
	<xsl:param name="Status"/>
	<xsl:param name="Package"/>
	
	<span>
		<xsl:attribute name="class"><xsl:value-of select="$Status" /></xsl:attribute>
		Total tests <xsl:value-of select="$Status" />: <xsl:value-of select="count(//Package[@Name=$Package]//Test[Status=$Status])" />
	</span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Package" />Tests<xsl:value-of select="$Status" />')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;">
		<xsl:attribute name="Id"><xsl:value-of select="$Package" />Tests<xsl:value-of select="$Status" /></xsl:attribute>
		<xsl:attribute name="class"><xsl:value-of select="$Status" /></xsl:attribute>
		<table width="100%">
			<tr>
				<th width="15%">Test Set</th>
				<th width="15%">Test</th>
				<th width="35%">Description</th>
				<th width="35%">Output</th>
			</tr>
		<xsl:for-each select="//Package[@Name=$Package]//Test[Status=$Status]" >				
			<tr>
				<td width="15%"><xsl:value-of select="../@Name" /></td>
				<td width="15%"><xsl:value-of select="@Name" /></td>
				<td width="35%"><xsl:value-of select="Desc" /></td>
				<td width="35%"><xsl:value-of select="Output" /></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
</xsl:template>

<xsl:template match="//Package">
<div>	
	<xsl:variable name="Package" select="@Name" />
	<H2>Package: <xsl:value-of select="@Name" /></H2>
	<H3>Duration: <xsl:value-of select="//FS/Duration" />s</H3>
	
	
	<span>Total tests: <xsl:value-of select="count(//Package[@Name=$Package]//Test)" /></span> <br/>
	<span class="Success">Total tests passed: <xsl:value-of select="count(//Package[@Name=$Package]//Test[Status='Success'])" /></span> <br/>
	<span class="Shallow">Total shallow tests: <xsl:value-of select="count(//Package[@Name=$Package]//Test[Status='Shallow'])" /></span> <br/>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Fatal</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Signaled</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Failed</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<!--xsl:call-template name="Summary">
		<xsl:with-param name="Status">FSimSuccess</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">FSimFail</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template-->
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Unresolved</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Unsupported</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Skipped</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>
	<xsl:call-template name="Summary">
		<xsl:with-param name="Status">Timeout</xsl:with-param>
		<xsl:with-param name="Package" select="$Package" />
	</xsl:call-template>	
	<br /><br />
	
	<h2>Details</h2>
	
	<xsl:for-each select="//Package[@Name=$Package]/TestSet" >		
				
		<xsl:variable name="TestSet" select="@Name" />
		<xsl:variable name="TestsTotal" select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test)" />
		<xsl:variable name="TestsNotPassed" select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status!='Success' and Status!='Shallow'])" />
		<xsl:variable name="TestsPassed" select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Success' or Status='Shallow'])" />
		<span class="Success">			
			<xsl:if test="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Timeout'])">
				<xsl:attribute name="class">Timeout</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Unsupported'])">
				<xsl:attribute name="class">Unsupported</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Unresolved'])">
				<xsl:attribute name="class">Unresolved</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Failed'])">
				<xsl:attribute name="class">Failed</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Fatal'])">
				<xsl:attribute name="class">Fatal</xsl:attribute>
			</xsl:if>
			<xsl:choose>
				<xsl:when test="$TestsNotPassed != 0 or count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Output!=''])">
					<a href="#" style="text-decoration:none">
						<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Package" />_<xsl:value-of select="$TestSet" />')</xsl:attribute> +</a>
				</xsl:when>
				<xsl:otherwise>&#160;&#160;&#160;</xsl:otherwise>
			</xsl:choose>
			
		<xsl:value-of select="$TestSet" />
		(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
		</span>		
		<br />
		<div style="border: solid 1px black; padding: 5px; display:none;">	
			<xsl:attribute name="id"><xsl:value-of select="$Package" />_<xsl:value-of select="$TestSet" /></xsl:attribute>
			Total Tests: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test)" /><br />
			<span class="Success">Tests passed: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Success'])" /></span> <br/>
			<span class="Shallow">Shallow tests: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Shallow'])" /></span> <br/>
			<span class="Failed">Tests failed: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Failed'])" /></span> <br/>
			<span class="Signaled">Tests signaled: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Signaled'])" /></span> <br/>
			<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Unresolved'])" /></span> <br/>
			<span class="FSimSuccess">Tests fault_sim failed: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='FSimSuccess'])" /></span> <br/>
			<span class="FSimFail">Tests fault_sim passed: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='FSimFail'])" /></span> <br/>
			<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Unsupported'])" /></span> <br/>
			<span class="Timeout">Tests timed out: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Timeout'])" /></span> <br/>
			<span class="Fatal">Tests fatal: <xsl:value-of select="count(//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test[Status='Fatal'])" /></span> <br/>
			<hr />
			<table width="100%">
				<tr>
				<th width="5%">Test Set</th>
				<th width="15%">Test</th>
				<th width="35%">Description</th>
				<th width="35%">Output</th>
			</tr>
				<xsl:for-each select="//Package[@Name=$Package]/TestSet[@Name=$TestSet]/Test">
					<xsl:if test="Status!='Success' and Status!='Shallow' or Output!=''">
						<tr>
							<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
							<td width="15%"><xsl:value-of select="../@Name" /></td>
							<td width="15%"><xsl:value-of select="@Name" /></td>
							<td width="35%"><xsl:value-of select="Desc" /></td>
							<td width="35%"><pre><xsl:value-of select="Output" /></pre></td>
						</tr>
					</xsl:if>
				</xsl:for-each>
			</table>
		</div>
	</xsl:for-each>	
</div>
</xsl:template>

</xsl:stylesheet>
