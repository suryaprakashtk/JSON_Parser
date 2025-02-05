//
// Created by Mark on 1/30/2024.
//

#pragma once

#include <string>
#include <optional>
#include "JSONParser.h"
#include <variant>
#include <map>
#include <vector>

namespace ECE141 {

	class ModelQuery; // Forward declare

	// STUDENT: Your Model is built from a bunch of these...
	class ModelNode {
		public:
			struct NullType{};
			using ListType = std::vector<ModelNode*>;
			using ObjectType = std::map<std::string, ModelNode*>;
			using Variant = std::variant<bool, long, double, std::string, NullType, ListType, ObjectType>;
				
			ModelNode(Variant inputVal, ModelNode* parentPtr = nullptr, std::string contName = "noName")
			{
				val = inputVal;
				containerName = contName;
				parentnode = parentPtr;
			}
			// ~ModelNode();
			ModelNode(const ModelNode& aModelNode)
			{
				*this = aModelNode;
			}
			ModelNode& operator=(const ModelNode& aModelnode)
			{
				this->val = aModelnode.val;
				this->containerName = aModelnode.containerName;
				this->childnodes = aModelnode.childnodes;
				this-> parentnode = aModelnode.parentnode;
				return *this;
			}


			// Sometimes a node holds a basic value (null, bool, number, string)
			// Sometimes a node holds a list of other nodes (list)
			// Sometimes a node holds a collection key-value pairs, where the value is a node (an object)
			Variant val;
			std::string containerName;
			std::vector<ModelNode*> childnodes;
			ModelNode* parentnode;
	};

	class Model : public JSONListener {
	public:
		Model();
		~Model() override = default;
		Model(const Model& aModel);
		Model &operator=(const Model& aModel);

		ModelQuery createQuery();
		ModelNode* rootnode;

	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;

		// STUDENT: Your model will contain a collection of ModelNode*'s
		//          Choose your container(s) wisely
		std::stack<ModelNode*> currentstack;
		
	};

	class ModelQuery {
	public:
		ModelQuery(Model& aModel);

		// ---Traversal---
		ModelQuery& select(const std::string& aQuery);

		// ---Filtering---
		ModelQuery& filter(const std::string& aQuery);

		// ---Consuming---
		size_t count();
		double sum();
		std::optional<std::string> get(const std::string& aKeyOrIndex);

	protected:
		Model& model;
		ModelNode* selectedNode;
		ModelNode::ListType filteredList;
		ModelNode::ObjectType filteredObject;
		bool isFilterApplied = false;
		bool isFilterAppliedObj = false;
	};


} // namespace ECE141