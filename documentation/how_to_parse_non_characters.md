# How to parse non-characters

## customizing the input type

Although the default implementation of the `class parse_context` has an `std::string` as the default input, it can be customized via templates. The signature of the `class parse_context` is the following:

	template <typename Input = std::string, typename Tag = std::string_view> class parse_context;

By default, parsing is for strings, but any input type can be used that conforms to the std container interface.

#customizing the output type

The `class parse_context` has another parameter `tag`. This parameter defines the type of tag each match will have. For example, if we wish to have an enumeration of our types, we would define our parse context like this:

	enum class element_type
	{
		string,
		number,
		object,
		array,
		boolean,
		null
	}; 

	typedef parserlib::parse_context<std::string, element_type> parse_context;
	typedef parserlib::rule<parse_context> rule;

We could then write our grammar by passing enum values to matches, instead of strings:

	rule string = ...;
	rule number = ...;
	rule object = ...;
	rule array = ...;
	rule boolean = ...;
	rule null = ...;

	rule element = string  == element_type::string
                 | number  == element_type::number
                 | object  == element_type::object
                 | array   == element_type::array
                 | boolean == element_type::boolean,
                 | null    == element_type::nuil;

	rule grammar = *element;

# processing the output of parsing

Processing the output of parsing is a simple loop where each match's tag can be used to create the appropriate object.

For example:

	parse_context pc(input);
	const bool ok = parse(grammar, pc);
	
	for(const auto& match : pc.matches)
	{
		switch (match.tag)
		{
			case element_type::string:
				break;

			case element_type::number:
				break;

			...

			case element_type::null:
				break;
		}
	}	

Each match contains:

1. a begin position, which is the same type as the input const iterator type.
2. an end position (same type as above).
3. a tag (the type of which is given in parse context).

The match type is declared as a member of the `class parse_context`, and it is the `struct match` member of the `class parse_context`.

