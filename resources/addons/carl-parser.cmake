
ExternalProject_Add(
    carl-parser
	GIT_REPOSITORY https://github.com/smtrat/carl-parser.git
	GIT_TAG deploy-latest
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(carl-parser BINARY_DIR)

add_dependencies(addons carl-parser)
add_dependencies(carl-parser lib_carl)

add_test(NAME carl-parser COMMAND ${BINARY_DIR}/test/carl-parser-test)
