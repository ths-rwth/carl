/*
 * parser.h
 *
 *  Created on: 14 Apr 2016
 *      Author: hbruintjes
 */

#pragma once

#include "carl/util/parser/ExpressionParserResult.h"

carl::parser::ExpressionType<Polynomial> parse(const std::string& input);
