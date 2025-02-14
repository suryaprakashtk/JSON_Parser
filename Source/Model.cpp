#include "Model.h"
#include "Debug.h"
#include <sstream>
#include "Formatting.h"

namespace JSONParserNamespace {

	// ---Model---

	Model::Model() {
		rootnode = nullptr;
	}

	Model::Model(const Model& aModel) {
		*this = aModel;
	}

	Model& Model::operator=(const Model& aModel) {
		this->rootnode = aModel.rootnode;
		this->currentstack = aModel.currentstack;
		return *this;
	}

	ModelQuery Model::createQuery() {
		return ModelQuery(*this);
	}

	bool Model::addKeyValuePair(const std::string& aKey, const std::string& aValue, Element aType) {
		// Print statement for debugging, remove after implementation
		DBG("\t'" << aKey << "' : '" << aValue << "'");
		ModelNode* currentContainer = currentstack.top();
		ModelNode::ObjectType& Map = std::get<ModelNode::ObjectType>(currentContainer->val);
		ModelNode* newNode;
		if(aType == Element::quoted)
		{
			newNode = new ModelNode(aValue, currentContainer);
			Map[aKey] = newNode;
			currentContainer->childnodes.push_back(newNode);
		}
		else if(aType == Element::constant)
		{
			if(aValue == "null")
			{
				ModelNode::NullType nullVariable;
				newNode = new ModelNode(nullVariable, currentContainer);
				Map[aKey] = newNode;
				currentContainer->childnodes.push_back(newNode);
			}
			else if(aValue == "true")
			{
				bool tempBool1 = true;
				newNode = new ModelNode(tempBool1, currentContainer);
				Map[aKey] = newNode;
				currentContainer->childnodes.push_back(newNode);
			}
			else if(aValue == "false")
			{
				bool tempBool2 = false;
				newNode = new ModelNode(tempBool2, currentContainer);
				Map[aKey] = newNode;
				currentContainer->childnodes.push_back(newNode);
			}
			else
			{
				double tempOther = std::stod(aValue);
				newNode = new ModelNode(tempOther, currentContainer);
				Map[aKey] = newNode;
				currentContainer->childnodes.push_back(newNode);
			}
		}
		return true;
	}

	bool Model::addItem(const std::string& aValue, Element aType) {
		// Print statement for debugging, remove after implementation
		DBG("\t'" << aValue << "'");
		ModelNode* currentContainer = currentstack.top();
		ModelNode::ListType& List = std::get<ModelNode::ListType>(currentContainer->val);
		ModelNode* newNode;
		if(aType == Element::quoted)
		{
			newNode = new ModelNode(aValue, currentContainer);
			List.push_back(newNode);
			currentContainer->childnodes.push_back(newNode);
		}
		else if(aType == Element::constant)
		{
			if(aValue == "null")
			{
				ModelNode::NullType nullVariable;
				newNode = new ModelNode(nullVariable, currentContainer);
				List.push_back(newNode);
				currentContainer->childnodes.push_back(newNode);
			}
			else if(aValue == "true")
			{
				bool tempBool1 = true;
				newNode = new ModelNode(tempBool1, currentContainer);
				List.push_back(newNode);
				currentContainer->childnodes.push_back(newNode);
			}
			else if(aValue == "false")
			{
				bool tempBool2 = false;
				newNode = new ModelNode(tempBool2, currentContainer);
				List.push_back(newNode);
				currentContainer->childnodes.push_back(newNode);
			}
			else
			{
				double tempOther = std::stod(aValue);
				newNode = new ModelNode(tempOther, currentContainer);
				List.push_back(newNode);
				currentContainer->childnodes.push_back(newNode);
			}
		}
		return true;
	}

	bool Model::openContainer(const std::string& aContainerName, Element aType) {
		DBG((aContainerName.empty() ? "EMPTY" : aContainerName) << " " << (aType == Element::object ? "{" : "["));
		ModelNode* parent = nullptr;
		if(!currentstack.empty())
			parent = currentstack.top();
		ModelNode* newNode;
		if(aType == Element::object)
		{
			ModelNode::ObjectType tempMap;
			newNode = new ModelNode(tempMap, parent, aContainerName);
		}
		else
		{
			ModelNode::ListType tempList;
			newNode = new ModelNode(tempList, parent, aContainerName);
		}

		if(currentstack.empty())
			rootnode = newNode;
		else
		{
			if(std::holds_alternative<ModelNode::ObjectType>(newNode->parentnode->val))
			{
				ModelNode::ObjectType& Map = std::get<ModelNode::ObjectType>(newNode->parentnode->val);
				Map[aContainerName] = newNode;
			}
			else if(std::holds_alternative<ModelNode::ListType>(newNode->parentnode->val))
			{
				ModelNode::ListType& List = std::get<ModelNode::ListType>(newNode->parentnode->val);
				List.push_back(newNode);
			}
			newNode->parentnode->childnodes.push_back(newNode);
		}
	
		currentstack.push(newNode);
		return true;
	}

	bool Model::closeContainer(const std::string& aContainerName, Element aType) {
		// Print statement for debugging, remove after implementation
		DBG(" " << (aType == Element::object ? "}" : "]"));
		currentstack.pop();
		std::string temp = aContainerName;
		return true;
	}


	// ---ModelQuery---

	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {
		selectedNode = model.rootnode;
	}

	ModelQuery& ModelQuery::select(const std::string& aQuery) {
		DBG("select(" << aQuery << ")");
		selectedNode = model.rootnode;
		std::istringstream iss(aQuery);
		std::vector<std::string> substrings;

		// Tokenize the string using getline with '.' as a delimiter
		std::string token;
		while (std::getline(iss, token, '.')) {
			// Remove leading and trailing single quotes
			size_t start = (token.front() == '\'') ? 1 : 0;
			size_t end = (token.back() == '\'') ? token.length() - 1 : token.length();
			substrings.push_back(token.substr(start, end - start));
		}

		// Print the vector of substrings
		for (const auto& substring : substrings) {
			for (auto it = (selectedNode->childnodes).begin(); it != (selectedNode->childnodes).end(); ++it)
			{
				if((*it)->containerName == substring)
				{
					selectedNode = *it;
					break;
				}
				else
				{
					try
					{
						int convertedValue = std::stoi(substring);
						if(std::holds_alternative<ModelNode::ListType>(selectedNode->val))
						{
							ModelNode::ListType& selectedList = std::get<ModelNode::ListType>(selectedNode->val);
							selectedNode = selectedList[convertedValue];
							break;
						}
					}
					catch(const std::invalid_argument& e){continue;}
				}
			}
		}

		
		return *this;
	}

	ModelQuery& ModelQuery::filter(const std::string& aQuery) {
		DBG("filter(" << aQuery << ")");
		std::string temp = "index";
		std::string temp2 = "key contains";
		if(temp == aQuery.substr(0,5))
		{
			filteredList = std::get<ModelNode::ListType>(selectedNode->val);
			isFilterApplied = true;
			if(aQuery.substr(6,2) == "==")
			{
				size_t index = std::stoi(aQuery.substr(aQuery.size() - 1,1));
				if(index < filteredList.size())
					filteredList = {filteredList[index]};
				else
					filteredList.erase(filteredList.begin(), filteredList.end());
			}
			else if(aQuery.substr(6,2) == "!=")
			{
				size_t index = std::stoi(aQuery.substr(aQuery.size() - 1,1));
				size_t startIndex = index;
				size_t endIndex = index + 1;
				if (startIndex < filteredList.size() && endIndex < filteredList.size() && startIndex <= endIndex) 
				{
					filteredList.erase(filteredList.begin() + startIndex, filteredList.begin() + endIndex);
				}
			}
			else if(aQuery.substr(6,2) == ">=")
			{
				size_t index = std::stoi(aQuery.substr(aQuery.size() - 1,1));
				size_t startIndex = 0;
				size_t endIndex = index;
				if (startIndex < filteredList.size() && endIndex < filteredList.size() && startIndex <= endIndex) 
				{
					filteredList.erase(filteredList.begin() + startIndex, filteredList.begin() + endIndex);
				}
			}
			else if(aQuery.substr(6,2) == "<=")
			{
				size_t index = std::stoi(aQuery.substr(aQuery.size() - 1,1));
				size_t startIndex = index;
				size_t endIndex = filteredList.size() - 1;
				if (startIndex < filteredList.size() && endIndex < filteredList.size() && startIndex <= endIndex) 
				{
					filteredList.erase(filteredList.begin() + startIndex, filteredList.begin() + endIndex);
				}
			}
			else if(aQuery.substr(6,2) == "> ")
			{
				size_t index = std::stoi(aQuery.substr(aQuery.size() - 1,1));
				size_t startIndex = 0;
				size_t endIndex = index;
				if (startIndex < filteredList.size() && endIndex < filteredList.size() && startIndex <= endIndex) 
				{
					filteredList.erase(filteredList.begin() + startIndex, filteredList.begin() + endIndex + 1);
				}
				else if(endIndex >= filteredList.size())
				{
					filteredList.erase(filteredList.begin() + startIndex, filteredList.end());
				}
				
			}
			else if(aQuery.substr(6,2) == "< ")
			{
				size_t index = std::stoi(aQuery.substr(aQuery.size() - 1,1));
				if (index < filteredList.size()) {
					// Erase elements after the specified index
					filteredList.erase(filteredList.begin() + index, filteredList.end());
				}
			}
		}
		else if(temp2 == aQuery.substr(0,12))
		{
			filteredObject = std::get<ModelNode::ObjectType>(selectedNode->val);
			isFilterAppliedObj = true;
			std::string keyVal = aQuery.substr(14,aQuery.size() - 14 - 1);

			auto it = filteredObject.begin();
			while (it != filteredObject.end()) {
				// Check if the key partially matches the given string
				if (it->first.find(keyVal) == std::string::npos) {
					// Erase the element if the key doesn't match
					it = filteredObject.erase(it);
				} else {
					// Move to the next element
					++it;
				}
			}		
		}
		return *this;
	}

	size_t ModelQuery::count() {
		DBG("count()");
		if(!filteredList.empty() || isFilterApplied)
			return filteredList.size();
		else if(!filteredObject.empty() || isFilterAppliedObj)
			return filteredObject.size();

		return (selectedNode->childnodes).size();
	}
	std::optional<std::string> getValue(ModelNode* selectedValue)
	{
		std::string result;
		if(selectedValue==nullptr)
			return std::nullopt;
		
		if(std::holds_alternative<ModelNode::ObjectType>(selectedValue->val))
		{
			ModelNode::ObjectType& selectedMap = std::get<ModelNode::ObjectType>(selectedValue->val);
			result = "{\"" ;
			for (const auto& pair : selectedMap) {
				
				std::optional<std::string> secondStr = getValue(pair.second);
				std::string stringValue;
				if(secondStr.has_value())
				{
					stringValue = secondStr.value();
				}
				else
				{
					return std::nullopt;
				}
				result += pair.first + "\":" + stringValue + ",\"";
			}
			result.erase(result.size() - 2);
			result += "}";
		}
		else if(std::holds_alternative<ModelNode::ListType>(selectedValue->val))
		{
			ModelNode::ListType& selectedList = std::get<ModelNode::ListType>(selectedValue->val);
			result = "[\"";
			for (auto it = selectedList.begin(); it != selectedList.end(); ++it) {
				std::optional<std::string> firstStr = getValue(*it);
				std::string stringValue;
				if(firstStr.has_value())
				{
					stringValue = firstStr.value();
				}
				else
				{
					return std::nullopt;
				}
				result += stringValue + ",";
			}
			result.erase(result.size() - 1);
			result += "]";
		}
		else if(std::holds_alternative<bool>(selectedValue->val))
		{
			bool result = std::get<bool>(selectedValue->val);
			if(result)
				return "true";
			else
				return "false";
		}
		else if(std::holds_alternative<long>(selectedValue->val))
		{
			long result = std::get<long>(selectedValue->val);
			return doubleToString(result);
		}
		else if(std::holds_alternative<double>(selectedValue->val))
		{
			double result = std::get<double>(selectedValue->val);
			return doubleToString(result);
		}
		else if(std::holds_alternative<std::string>(selectedValue->val))
		{
			std::string result = std::get<std::string>(selectedValue->val);
			return "\"" + result + "\"";
		}
		else if(std::holds_alternative<ModelNode::NullType>(selectedValue->val))
		{
			return "null";
		}

		return result;
	}

	double ModelQuery::sum() {
		DBG("sum()");
		double result = 0.0;
		if(!filteredList.empty() || isFilterApplied)
		{
			for (auto it = filteredList.begin(); it != filteredList.end(); ++it) {
				std::optional<std::string> firstStr = getValue(*it);
				std::string stringValue;
				if(firstStr.has_value())
				{
					stringValue = firstStr.value();
				}
				else
				{
					return 0.0;
				}
				result = result + std::stod(stringValue);
			}
			return result;
		}
		else if(isFilterApplied)
			return result;
	
		if(std::holds_alternative<ModelNode::ListType>(selectedNode->val))
		{
			ModelNode::ListType& selectedList = std::get<ModelNode::ListType>(selectedNode->val);
			for (auto it = selectedList.begin(); it != selectedList.end(); ++it) {
				std::optional<std::string> firstStr = getValue(*it);
				std::string stringValue;
				if(firstStr.has_value())
				{
					stringValue = firstStr.value();
				}
				else
				{
					return 0.0;
				}
				result = result + std::stod(stringValue);
			}
		}
		return result;
	}

	
	
	std::optional<std::string> ModelQuery::get(const std::string& aKeyOrIndex) {
		DBG("get(" << aKeyOrIndex << ")");
		std::string temp;
		std::istringstream iss(aKeyOrIndex);
		std::vector<std::string> substrings;

		// Tokenize the string using getline with '.' as a delimiter
		std::string token;
		while (std::getline(iss, token, '.')) {
			// Remove leading and trailing single quotes
			size_t start = (token.front() == '\'') ? 1 : 0;
			size_t end = (token.back() == '\'') ? token.length() - 1 : token.length();
			substrings.push_back(token.substr(start, end - start));
		}

		// Print the vector of substrings
		for (const auto& substring : substrings) {
			temp = substring;
		}

		std::optional<std::string> result;
		if(temp == "*")
		{
			if(isFilterAppliedObj)
			{
				ModelNode tempFilter (filteredObject);
				result = getValue(&tempFilter);
			}
			else
				result = getValue(selectedNode);
		}
		else if (std::holds_alternative<ModelNode::ObjectType>(selectedNode->val)) 
		{
			ModelNode::ObjectType& selectedMap = std::get<ModelNode::ObjectType>(selectedNode->val);
			ModelNode* selectedKey = selectedMap[temp];
			result = getValue(selectedKey);
		}
		else
		{
			ModelNode::ListType& selectedList = std::get<ModelNode::ListType>(selectedNode->val);
			ModelNode* selectedKey = selectedList[std::stoi(temp)];
			result = getValue(selectedKey);
		}
		// restore selected node to rootnode
		selectedNode = model.rootnode;
		std::string output;
		if(result.has_value())
		{
			output = result.value();
			return output;
		}
		

		return result;
	}

	

}