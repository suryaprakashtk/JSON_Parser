# JSON Prcoessor
## Objective
In this project a JSON Parser, in-memory model and query command system was built. The main objectives were:
- Parsing JSON text files.
- Building an in-memory model of the JSON.
- Creating and using a simple command language for querying the in-memory model and retrieving data from the JSON.

## Code Usage
To run this code, clone the repo. Set the proper working directory in ```getWorkingDirectoryPath()``` function in the ```./Source/main.cpp``` file. 

The run the following command in the terminal from the working directory
```bash
mkdir build
cmake build
cd build/
```

To compile and check run
```bash
make
./JSON_Parser compile
```
if the result appears to be
```bash
Test 'compile' PASSED
```
The code is ready for use.

A few test cases on the query system is designed in the `./Test/` text files. 

Any JSON can be parsed. Change the following in the `main.cpp` file. 
For example:

```cpp
// Build the model
std::fstream theJsonFile(getWorkingDirectoryPath() + "/Resources/classroom.json");
JSONParserNamespace::JSONParser theParser(theJsonFile);
JSONParserNamespace::Model theModel;
theParser.parse(&theModel);

// Query the model
auto theQuery = theModel.createQuery();
auto theResult = theQuery.select("'sammy'").get("'username'");
```

## Implementation Details
A JSON parser is provided as input. The goal is to build a `Model` which inherits from `JSONListener` and implements their pure virtual methods in order for the JSON parser to correctly parse the input file. These listener methods will be called by the parser each time a new JSON element is parsed and the element gets added to the `Model`. 

### In-Memory Model
For something as dynamic and flexible as JSON, a "graph" model was built to store the various JSON values/elements. Each value/element in the JSON can be considered a "node" in our graph. Consider the below example:
```json 
{
  "stringNode": "Hello!",
  "numberNode": 42,
  "objectNode": {
    "innerStringNode": "Hello again!",
    "innerBoolNode": true,
    "innerListNode": [
      "item1", "item2", "item3"
    ]
  }
}
```
The first character we see is the open-curly-brace `{`. This character is associated with the start of an object type.

Since the root element of JSON is always an object, we know our graph will always start with an object.

So we can add an object to the root of our graph:

```
node(object) -> ...
```
Looking inside this root `object` at the key-value pairs. 
The first one is `"stringNode": "Hello!"`, 
where the key is `"stringNode"` and the value is the string `"Hello!"`. Similarly a graph like below is built.

```
node(object) -> {
    key: "stringNode", value -> node(string)
    key: "numberNode", value -> node(int)
    key: "objectNode", value -> node(object) -> {
        key: "innerStringNode", value -> node(string)
        key: "innerBoolNode", value -> node(bool)
        key: "innerListNode", value -> node(list) -> {
            node(string), node(string), node(string)
        }
    }
}
```
This is the graph representation of the JSON file. The STL used was a vector of pointers to each node.

### Model Query Functions
Query syntax like the one below is used to navigate between nodes in the JSON model.

```cpp
ModelQuery& select(const std::string& aQuery);
```
Filtering is used to 'skip' or 'ignore' certain JSON elements.

```cpp
ModelQuery& filter(const std::string& aQuery);
```

After navigating and filtering the JSON, 
these commands will be used at the end of the command chain to actually return some data/values.

```cpp
size_t count(); //Counts number of child elements of the currently selected node.
double sum(); // Sum values in a list. This will only be used in lists of numbers.

std::optional<std::string> get(const std::string& aKeyOrIndex);
// Get values of a certain key-value pair or value at index of a list.
// Passing "*" as the argument will return all child nodes of the currently selected node.
```

### Example Usage:

```json
{
  "sammy": {
    "name": "Sammy",
    "username": "SammyShark",
    "online": true,
    "followers": {
      "avg-age": 25,
      "count": 100
    }
  },
  "items": [
    {"key1": "100"}
  ],
  "list": [
    100, 250, 3000
  ]
}
```

- `select("'sammy'").get("'username'")`: Output is `"SammyShark"`.

- `select("'sammy'.'uh_oh'").get("'nope'")`: Output is `std::nullopt`, as the JSON does not contain these elements.

- `select("'list'").sum()`: Output is `3350`.

- `select("'list'").filter("index >= 1").sum()`: Output is `3250`.

- `select("'sammy'").count()`: Output is `4`.

- `select("'sammy'.'followers'").get("*")`: Output is `{"avg-age":25,"count":100}`.

- `select("'sammy'.'followers'").get("'count'")`: Output is `100`.

- `select("'items'").get("0")`: Output is `{"key1":"100"}`.

- `select("'list'").filter("index < 2").get("*")`: Output is `[100,250]`.

- `select("'sammy'").filter("key contains 'name').get("*")`: Output is `{"name":"Sammy","username":"SammyShark"}` since these are the only key-value pairs within the `"sammy"` object that have keys containing the substring `"name"`.

## Acknowledgement
This project was done as a part of coursework at UCSD.