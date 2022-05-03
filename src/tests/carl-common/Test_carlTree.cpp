#include "gtest/gtest.h"

#include <carl-common/datastructures/carlTree.h>

using namespace carl;

TEST(carlTree, Basic)
{
	carl::tree<int> t;
	auto i1 = t.append(1);
	auto i2 = t.append(2);
	auto i3 = t.append(3);
	t.insert(i1, 4);
	t.insert(i1, 5);
	t.insert(i2, 6);
	t.insert(i2, 7);
	t.insert(i3, 8);
	t.erase(i2);
	std::cout << t << std::endl;
	std::cout << std::endl << "Preorder ";
	for (auto i = t.begin_preorder(); i != t.end_preorder(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "RPreorder ";
	for (auto i = t.rbegin_preorder(); i != t.rend_preorder(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "Postorder ";
	for (auto i = t.begin_postorder(); i != t.end_postorder(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "RPostorder ";
	for (auto i = t.rbegin_postorder(); i != t.rend_postorder(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "Leaf ";
	for (auto i = t.begin_leaf(); i != t.end_leaf(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "RLeaf ";
	for (auto i = t.rbegin_leaf(); i != t.rend_leaf(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "Depth ";
	for (auto i = t.begin_depth(2); i != t.end_depth(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "RDepth ";
	for (auto i = t.rbegin_depth(2); i != t.rend_depth(); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "Children ";
	for (auto i = t.begin_children(i1); i != t.end_children(i1); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "RChildren ";
	for (auto i = t.rbegin_children(i1); i != t.rend_children(i1); ++i) std::cout << *i << ", ";
	std::cout << std::endl << "Path ";
	for (auto i = t.begin_path(i1); i != t.end_path(); ++i) std::cout << *i << ", ";
	std::cout << std::endl;
}
