#ifndef XML_SPRUCE_LOG_HPP
#define XML_SPRUCE_LOG_HPP

#include <Test.hpp>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

class XMLGenerator
{
public:
	static string Validate(string str)
	{
		StrReplace(str, "&", "&amp;");
		StrReplace(str, "<", "&lt;");
		StrReplace(str, ">", "&gt;");
		StrReplace(str, "'", "&apos;");
		StrReplace(str, "\"", "&quot;");
		return str;
	}
	
	// Generates the log file
	static void GenerateLog(const string & file_name, const TestPackage & results, string fs, string mount_opts, string duration)
	{
		ofstream of(file_name.c_str(), ios_base::app);
		string xml = "";
		xml += "<SpruceLog>";
		xml += "<FS Name=\"" + Validate(fs) + "\" MountOptions=\"" + Validate(mount_opts) + "\">\n";
		xml += "\t<Package Name=\"" + Validate(results.GetName()) + "\">\n";
		of << xml;
		
		map<string, TestSet> TestSetResults = results.GetTestSets();
		
		for ( map<string, TestSet>::iterator i = TestSetResults.begin(); i != TestSetResults.end(); ++i )
		{
			xml = "\t\t<TestSet Name=\"" + Validate((*i).second.GetName()) + "\">\n";
			
			map<string, Test> Tests = (*i).second.GetTests();
			
			for ( map<string, Test>::iterator j = Tests.begin(); j != Tests.end(); ++j )
			{
				stringstream str;
				str << rand();
				
				xml += "\t\t\t<Test Name=\"" + Validate((*j).second.GetName()) + "\" Id=\"" + Validate(str.str()) + "\">\n";				
				xml += "\t\t\t\t<Desc>" + Validate((*j).second.GetDescription()) + "</Desc>\n";
				xml += "\t\t\t\t<Results>\n";
				
				map<Checks, TestResult> Results = (*j).second.GetResults();
				
				for ( map<Checks, TestResult>::iterator k = Results.begin(); k != Results.end(); ++k )
				{
					xml += "\t\t\t\t\t<Check Name=\"" + k->second.CheckToString(k->first) + "\">\n";
					xml += "\t\t\t\t\t\t<Status>" + Validate((*k).second.StatusToString()) + "</Status>\n";
					xml += "\t\t\t\t\t\t<Output>" + Validate((*k).second.GetOutput()) + "</Output>\n";
					xml += "\t\t\t\t\t</Check>\n";
					
				}
				
				xml += "\t\t\t\t</Results>\n";
				xml += "\t\t\t</Test>\n";
			}
			
			xml += "\t\t</TestSet>";
			of << xml;
		}
		
		xml = "";
		xml += "\t</Package>\n";
		xml += "\t<Duration>" + Validate(duration) + "</Duration>\n";
		xml += "</FS>";
		xml += "</SpruceLog>";
		of << xml;
		of.close();
	}
};

class Item
{
public:
	Item(string name = "", string operation = "", string description="", string status = "", string output = "", string arguments = ""):
		name_(name),
		id_(""),
		operation_(operation),
		status_(status),
		output_(output),
		arguments_(arguments),
		description_(description),
		xml_(""),
		validXml_(false)
	{
		stringstream str;
		str << rand();
		id_ = str.str();
	}
	
	Item(const Item &item):
		name_(item.getName()),
		id_(item.getId()),
		operation_(item.getOperation()),		
		status_(item.getStatus()),
		output_(item.getOutput()),
		arguments_(item.getArguments()),
		description_(item.getDescription()),
		xml_(""),
		validXml_(item.getValidXml())
	{	
		
	}
	
	virtual ~Item()
	{
	}
	
	string getXml()
	{
		genXml();
		return xml_;
	}
	
	string getName() const
	{
		return name_;
	}
	string getId() const
	{
		return id_;
	}
	string getOperation() const
	{
		return operation_;
	}
	string getStatus() const
	{
		return status_;
	}
	string getOutput() const
	{
		return output_;
	}
	string getArguments() const
	{
		return arguments_;
	}
	string getDescription() const
	{
		return description_;
	}
	
	bool getValidXml() const
	{
		return validXml_;
	}
	
	void setName(string name)
	{
		validXml_ = false;
		name_ = name;
	}	

	void setOperation(string operation)
	{
		validXml_ = false;
		operation_ = operation;
	}
	void setStatus(string status)
	{
		validXml_ = false;
		status_ = status;
	}
	void setOutput(string output)
	{
		validXml_ = false;
		output_ = output;
	}
	void appendOutput(string output)
	{
		validXml_ = false;
		output_ += output;
	}
	void setArguments(string arguments)
	{
		validXml_ = false;
		arguments_ = arguments;
	}
	void setDescription(string description)
	{
		validXml_ = false;
		description_ = description;
	}
	
protected:
	string name_;
	string id_;
	string operation_;
	string status_;
	string output_;
	string arguments_;
	string description_;
	
	string xml_;
	bool validXml_;
	
	void processValidXml()
	{
		processValidTag(operation_);
		processValidTag(status_);
		processValidTag(output_);
		processValidTag(arguments_);
		processValidTag(description_);
	}
	
	void processValidTag(string& str)
	{
		replace(str, "&", "&amp;");
		replace(str, "<", "&lt;");
		replace(str, ">", "&gt;");
		replace(str, "'", "&apos;");
		replace(str, "\"", "&quot;");
	}
	
	void replace(string& str, string val1, string val2)
	{
		StrReplace(str, val1, val2);
	}
	
	void genXml()
	{
		if(!validXml_)
		{			
			processValidXml();
			xml_ = "";
			xml_ += "\t\t<Item Name=\"" + name_ + "\"" + " Id=\"" + id_ + "\">\n";
			xml_ += "\t\t\t<Operation>" + operation_ + "</Operation>\n";
			xml_ += "\t\t\t<Description>" + description_ + "</Description>\n";
			xml_ += "\t\t\t<Status>" + status_ + "</Status>\n";
			xml_ += "\t\t\t<Output>" + output_ + "</Output>\n";
			xml_ += "\t\t\t<Arguments>" + arguments_ + "</Arguments>\n";
			xml_ += "\t\t</Item>\n";
			validXml_ = true;
		}
	}
};

class ModuleLog
{
public:
	ModuleLog(string fileName): fileName_(fileName), items_(vector<Item>())
	{

	}
	~ModuleLog()
	{
	}

	void addItem(Item item)
	{
		items_.push_back(item);
		ofstream of(fileName_.c_str(), ios_base::app);		
		of << item.getXml();
		of.close();
	}

protected:
	string fileName_;
	vector<Item> items_;
};

class SpruceLog
{
public:

	SpruceLog(string fileName = "", string fsName = "", string mountOptions = "", string duration = "", string moduleName = ""): fileName_(fileName), fsName_(fsName), mountOptions_(mountOptions), duration_(duration), moduleName_(moduleName)
	{
		
	}
	
	void setDuration(string duration)
	{
		duration_ = duration;
	}
	
	virtual ~SpruceLog()
	{

	}
	
	void openTags()
	{
		ofstream of(fileName_.c_str(), ios_base::app);
		string xml = "";
		xml += "<SpruceLog>";
		xml += "<FS Name=\"" + fsName_ + "\" MountOptions=\"" + mountOptions_ + "\">\n";
		xml += "\t<Module Name=\"" + moduleName_ + "\">\n";
		of << xml;
		of.close();
	}
	
	void closeTags()
	{
		ofstream of(fileName_.c_str(), ios_base::app);
		string xml = "";
		xml += "\t</Module>\n";
		xml += "\t<Duration>" + duration_ + "</Duration>\n";
		xml += "</FS>";
		xml += "</SpruceLog>";
		of << xml;
		of.close();
	}

protected:
	string fileName_;	
	string fsName_;
	string mountOptions_;
	string duration_;
	string moduleName_;
};
#endif
