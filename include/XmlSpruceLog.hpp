#ifndef XML_SPRUCE_LOG_HPP
#define XML_SPRUCE_LOG_HPP

#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

class Item
{
public:
	Item(string name = "", string operation = "", string status = "", string output = "", string arguments = ""): name_(name), id_(""), operation_(operation), status_(status), output_(output), arguments_(arguments), xml_(""), validXml_(false)
	{
		stringstream str;
		str << rand();
		id_ = str.str();
	}
	
	Item(const Item &item): name_(item.getName()), id_(item.getId()), operation_(item.getOperation()), status_(item.getStatus()), output_(item.getOutput()), arguments_(item.getArguments()), xml_(""), validXml_(item.getValidXml())
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
	
protected:
	string name_;
	string id_;
	string operation_;
	string status_;
	string output_;
	string arguments_;
	
	string xml_;
	bool validXml_;
	
	void processValidXml()
	{
		processValidTag(operation_);
		processValidTag(status_);
		processValidTag(output_);
		processValidTag(arguments_);
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
		size_t pos = -1;
		while (true)
		{
			pos = str.find(val1.c_str(), pos + 1);
			if ( pos == string::npos )
				break;				
			str.replace(pos, 1, val2.c_str());
		}	
	}
	
	void genXml()
	{
		if(!validXml_)
		{			
			processValidXml();
			xml_ = "";
			xml_ += "\t\t<Item Name=\"" + name_ + "\"" + " Id=\"" + id_ + "\">\n";
			xml_ += "\t\t\t<Operation>" + operation_ + "</Operation>\n";
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
