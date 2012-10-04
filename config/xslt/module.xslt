<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">



<xsl:template match="//Module">
<div>
	
	<xsl:variable name="Module" select="@Name" />
	<H2>Module: <xsl:value-of select="@Name" /></H2>
	
	<span class="Success">Total tests passed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Success'])" /></span> <br/>
	<span class="Shallow">Total shallow tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Shallow'])" /></span> <br/>
	<span class="Failed">Total tests failed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Failed'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsFailed')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Failed">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsFailed</xsl:attribute>
		<table width="100%">
			<tr>
				<th>Name</th>
				<th>Operation</th>
				<th>Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Failed']" >				
			<tr>
				<td><xsl:value-of select="@Name" /></td>
				<td><xsl:value-of select="Operation" /></td>
				<td><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Unresolved">Total tests unresolved: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Unresolved'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsUnresolved')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Unresolved">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsUnresolved</xsl:attribute>
		<table width="100%">
			<tr>
				<th>Name</th>
				<th>Operation</th>
				<th>Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Unresolved']" >				
			<tr>
				<td><xsl:value-of select="@Name" /></td>
				<td><xsl:value-of select="Operation" /></td>
				<td><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Unsupported">Total tests unsupported: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Unsupported'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsUnsupported')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Unsupported">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsUnsupported</xsl:attribute>
		<table width="100%">
			<tr>
				<th>Name</th>
				<th>Operation</th>
				<th>Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Unsupported']" >				
			<tr>
				<td><xsl:value-of select="@Name" /></td>
				<td><xsl:value-of select="Operation" /></td>
				<td><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Skipped">Total tests skipped: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Skipped'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsSkipped')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Skipped">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsSkipped</xsl:attribute>
		<table width="100%">
			<tr>
				<th>Name</th>
				<th>Operation</th>
				<th>Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Skipped']" >				
			<tr>
				<td><xsl:value-of select="@Name" /></td>
				<td><xsl:value-of select="Operation" /></td>
				<td><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br />
	<span class="Timeout">Total tests timed out: <xsl:value-of select="count(//Module[@Name=$Module]/Item[Status='Timeout'])" /></span>
	<a href="#" style="text-decoration:none">
		<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />TestsTimedOut')</xsl:attribute> +</a>
	<div style="border: solid 1px black; padding: 5px; display:none;" class="Timeout">
		<xsl:attribute name="Id"><xsl:value-of select="$Module" />TestsTimedOut</xsl:attribute>
		<table width="100%">
			<tr>
				<th>Name</th>
				<th>Operation</th>
				<th>Output</th>
			</tr>
		<xsl:for-each select="//Module[@Name=$Module]/Item[Status='Teimout']" >				
			<tr>
				<td><xsl:value-of select="@Name" /></td>
				<td><xsl:value-of select="Operation" /></td>
				<td><pre><xsl:value-of select="Output" /></pre></td>
			</tr>
		</xsl:for-each>
		</table>
	</div>
	<br /><br />
	
	<h2>Details</h2>
	
	<xsl:variable name="key_name"><xsl:value-of select="$Module" />s</xsl:variable>
	<!--count(. | key($key_name, @Name)[1]) = 1]" -->
	<xsl:for-each select="//Module[@Name=$Module]/Item[not(@Name=preceding-sibling::Item/@Name)]" >
		
		<xsl:sort select="@Name" />
		
		<xsl:variable name="Item" select="@Name" />
		<xsl:variable name="TestsTotal" select="count(//Module[@Name=$Module]/Item[@Name=$Item])" />
		<xsl:variable name="TestsPassed" select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Success']) + count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Shallow'])" />
		<span class="Success">			
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Timeout'])">
				<xsl:attribute name="class">Timeout</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unsupported'])">
				<xsl:attribute name="class">Unsupported</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unresolved'])">
				<xsl:attribute name="class">Unresolved</xsl:attribute>
			</xsl:if>
			<xsl:if test="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Failed'])">
				<xsl:attribute name="class">Failed</xsl:attribute>
			</xsl:if>
			<xsl:choose>
				<xsl:when test="$TestsTotal != $TestsPassed">
					<a href="#" style="text-decoration:none">
						<xsl:attribute name="onclick">ShowHideTest(this, '<xsl:value-of select="$Module" />_<xsl:value-of select="$Item" />')</xsl:attribute> +</a>
				</xsl:when>
				<xsl:otherwise>&#160;&#160;&#160;</xsl:otherwise>
			</xsl:choose>
			
		<xsl:value-of select="$Item" />
		(Passed : <xsl:value-of select="$TestsPassed" />/<xsl:value-of select="$TestsTotal" />)
		</span>		
		<br />
		<div style="border: solid 1px black; padding: 5px; display:none;">	
			<xsl:attribute name="id"><xsl:value-of select="$Module" />_<xsl:value-of select="$Item" /></xsl:attribute>
			<xsl:for-each select="//Module[@Name=$Module]/Item[@Name=$Item]">
				<xsl:if test="Status!='Success' and Status!='Shallow'">
					<div>
						<xsl:attribute name="class"><xsl:value-of select="Status" /></xsl:attribute>
						<table width="100%">
							<tr>
								<th>Name</th>
								<th>Operation</th>
								<th>Output</th>
							</tr>
							
							<tr>
								<td><xsl:value-of select="@Name" /></td>
								<td><xsl:value-of select="Operation" /></td>
								<td><pre><xsl:value-of select="Output" /></pre></td>
							</tr>
						</table>
					</div>
				</xsl:if>
			</xsl:for-each>
			<hr />
			Total Tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item])" /><br />
			<span class="Success">Tests passed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Success'])" /></span> <br/>
			<span class="Shallow">Shallow tests: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Shallow'])" /></span> <br/>
			<span class="Failed">Tests failed: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Failed'])" /></span> <br/>
			<span class="Unresolved">Tests unresolved: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unresolved'])" /></span> <br/>
			<span class="Unsupported">Tests unsupported: <xsl:value-of select="count(//Module[@Name=$Module]/Item[@Name=$Item and Status='Unsupported'])" /></span> <br/>
		</div>
	</xsl:for-each>	
	
	<hr />
	
	
</div>
</xsl:template>

</xsl:stylesheet>
