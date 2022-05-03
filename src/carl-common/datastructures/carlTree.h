/**
 * @file carlTree.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <cassert>
#include <iterator>
#include <list>
#include <limits>
#include <stack>
#include <type_traits>
#include <vector>

#include <carl-common/util/streamingOperators.h>


namespace carl {

template<typename T>
class tree;

namespace tree_detail {

constexpr std::size_t MAXINT = std::numeric_limits<std::size_t>::max();

template<typename T>
struct Node {
	std::size_t id;
	mutable T data;
	std::size_t parent;
	std::size_t previousSibling = MAXINT;
	std::size_t nextSibling = MAXINT;
	std::size_t firstChild = MAXINT;
	std::size_t lastChild = MAXINT;
	std::size_t depth = MAXINT;
	Node(std::size_t _id, T&& _data, std::size_t _parent, std::size_t _depth):
		id(_id), data(std::move(_data)), parent(_parent), depth(_depth)
	{}
};
template<typename T>
bool operator==(const Node<T>& lhs, const Node<T>& rhs) {
	return &lhs == &rhs;
}
template<typename T>
std::ostream& operator<<(std::ostream& os, const Node<T>& n) {
	int id = (int)n.id;
	int parent = (n.parent == MAXINT ? -1 : (int)n.parent);
	int firstChild = (n.firstChild == MAXINT ? -1 : (int)n.firstChild);
	int lastChild = (n.lastChild == MAXINT ? -1 : (int)n.lastChild);
	int previousSibling = (n.previousSibling == MAXINT ? -1 : (int)n.previousSibling);
	int nextSibling = (n.nextSibling == MAXINT ? -1 : (int)n.nextSibling);
	os << "(" << n.data << " @ " << id << ", " << parent << ", " << firstChild << ":" << lastChild << ", " << previousSibling << " <-> " << nextSibling << ")\n";

	return os;
}

/**
 * This is the base class for all iterators.
 * It takes care of correct implementation of all operators and reversion.
 *
 * An actual iterator `T<reverse>` only has to
 * - inherit from `BaseIterator<T, reverse>`,
 * - provide appropriate constructors,
 * - implement `next()` and `previous()`.
 * If the iterator supports only forward iteration, it omits the template
 * argument, inherits from `BaseIterator<T, false>` and does not implement
 * `previous()`.
 */
template<typename T, typename Iterator, bool reverse>
struct BaseIterator {
	template<typename TT, typename It, bool rev>
	friend struct BaseIterator;
protected:
	const tree<T>* mTree;
	BaseIterator(const tree<T>* t, std::size_t root): mTree(t), current(root) {}
public:
	const auto& nodes() const {
		return mTree->nodes;
	}
	const auto& node(std::size_t id) const {
		assert(id != MAXINT);
		return nodes()[id];
	}
	const auto& curnode() const {
		return node(current);
	}
	std::size_t current;
	BaseIterator(const BaseIterator& ii) = default;
	BaseIterator(BaseIterator&& ii) noexcept = default;
	template<typename It, bool r>
	BaseIterator(const BaseIterator<T,It,r>& ii): mTree(ii.mTree), current(ii.current) {}
	BaseIterator& operator=(const BaseIterator& ii) = default;
	BaseIterator& operator=(BaseIterator&& ii) noexcept = default;
	std::size_t depth() const {
		return node(current).depth;
	}
	std::size_t id() const {
		assert(current != MAXINT);
		return current;
	}
	bool isRoot() const {
		return current == 0;
	}
	bool isValid() const {
		return (mTree != nullptr) && mTree->is_valid(*this);
	}
	T* operator->() {
		return &(curnode().data);
	}
	T const * operator->() const {
		return &(curnode().data);
	}
};
template<typename T, typename I, bool r>
T& operator*(BaseIterator<T,I,r>& bi) {
	return bi.curnode().data;
}
template<typename T, typename I, bool r>
const T& operator*(const BaseIterator<T,I,r>& bi) {
	return bi.curnode().data;
}

template<typename T, typename I, bool reverse>
typename std::enable_if<!reverse,I>::type& operator++(BaseIterator<T,I,reverse>& it) {
	return static_cast<I*>(&it)->next();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<reverse,I>::type& operator++(BaseIterator<T,I,reverse>& it) {
	return static_cast<I*>(&it)->previous();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<!reverse,I>::type operator++(BaseIterator<T,I,reverse>& it, int) {
	return static_cast<I*>(&it)->next();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<reverse,I>::type operator++(BaseIterator<T,I,reverse>& it, int) {
	return static_cast<I*>(&it)->previous();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<!reverse,I>::type& operator--(BaseIterator<T,I,reverse>& it) {
	return static_cast<I*>(&it)->previous();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<reverse,I>::type& operator--(BaseIterator<T,I,reverse>& it) {
	return static_cast<I*>(&it)->next();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<!reverse,I>::type operator--(BaseIterator<T,I,reverse>& it, int) {
	return static_cast<I*>(&it)->previous();
}
template<typename T, typename I, bool reverse>
typename std::enable_if<reverse,I>::type operator--(BaseIterator<T,I,reverse>& it, int) {
	return static_cast<I*>(&it)->next();
}


template<typename T, typename I, bool r>
bool operator==(const BaseIterator<T,I,r>& i1, const BaseIterator<T,I,r>& i2) {
	return i1.current == i2.current;
}
template<typename T, typename I, bool r>
bool operator!=(const BaseIterator<T,I,r>& i1, const BaseIterator<T,I,r>& i2) {
	return i1.current != i2.current;
}
template<typename T, typename I, bool r>
bool operator<(const BaseIterator<T,I,r>& i1, const BaseIterator<T,I,r>& i2) {
	return i1.current < i2.current;
}

/**
 * Iterator class for pre-order iterations over all elements.
 */
template<typename T, bool reverse = false>
struct PreorderIterator:
	BaseIterator<T,PreorderIterator<T,reverse>, reverse>,
	std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
{
	using Base = BaseIterator<T,PreorderIterator<T,reverse>, reverse>;
	PreorderIterator(const tree<T>* t): Base(t, MAXINT) {}
	PreorderIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
	PreorderIterator& next() {
		if (this->current == MAXINT) {
			this->current = this->mTree->begin_preorder().current;
		} else if (this->curnode().firstChild == MAXINT) {
			while (this->curnode().nextSibling == MAXINT) {
				this->current = this->curnode().parent;
				if (this->current == MAXINT) return *this;
			}
			this->current = this->curnode().nextSibling;
		} else {
			this->current = this->curnode().firstChild;
		}
		return *this;
	}
	PreorderIterator& previous() {
		if (this->current == MAXINT) {
			this->current = this->mTree->rbegin_preorder().current;
		} else if (this->curnode().previousSibling == MAXINT) {
			this->current = this->curnode().parent;
		} else {
			this->current = this->curnode().previousSibling;
			while (this->curnode().firstChild != MAXINT) {
				this->current = this->curnode().lastChild;
			}
		}
		return *this;
	}

	template<typename It,bool rev>
	PreorderIterator(const BaseIterator<T,It,rev>& ii): Base(ii) {}
	PreorderIterator(const PreorderIterator& ii): Base(ii) {}
	PreorderIterator(PreorderIterator&& ii): Base(ii) {}
	PreorderIterator& operator=(const PreorderIterator& it) {
		Base::operator=(it);
		return *this;
	}
	PreorderIterator& operator=(PreorderIterator&& it) {
		Base::operator=(it);
		return *this;
	}
	virtual ~PreorderIterator() noexcept = default;
	
	PreorderIterator& skipChildren() {
		assert(this->current != MAXINT);
		while (this->curnode().nextSibling == MAXINT) {
			this->current = this->curnode().parent;
			if (this->current == MAXINT) return *this;
		}
		this->current = this->curnode().nextSibling;
		return *this;
	}
};
static_assert(std::is_copy_constructible<PreorderIterator<int,false>>::value, "");
static_assert(std::is_move_constructible<PreorderIterator<int,false>>::value, "");
static_assert(std::is_destructible<PreorderIterator<int,false>>::value, "");
static_assert(std::is_copy_constructible<PreorderIterator<int,true>>::value, "");
static_assert(std::is_move_constructible<PreorderIterator<int,true>>::value, "");
static_assert(std::is_destructible<PreorderIterator<int,true>>::value, "");

/**
 * Iterator class for post-order iterations over all elements.
 */
template<typename T, bool reverse = false>
struct PostorderIterator:
	BaseIterator<T, PostorderIterator<T, reverse>,reverse>,
	std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
{
	using Base = BaseIterator<T, PostorderIterator<T, reverse>,reverse>;
	PostorderIterator(const tree<T>* t): Base(t, MAXINT) {}
	PostorderIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
	PostorderIterator& next() {
		if (this->current == MAXINT) {
			this->current = this->mTree->begin_postorder().current;
		} else  if (this->curnode().nextSibling == MAXINT) {
			this->current = this->curnode().parent;
		} else {
			this->current = this->curnode().nextSibling;
			while (this->curnode().firstChild != MAXINT) {
				this->current =	this->curnode().firstChild;
			}
		}
		return *this;
	}
	PostorderIterator& previous() {
		if (this->current == MAXINT) {
			this->current = this->mTree->rbegin_postorder().current;
		} else if (this->curnode().firstChild == MAXINT) {
			if (this->curnode().previousSibling != MAXINT) {
				this->current = this->curnode().previousSibling;
			} else {
				while (this->curnode().previousSibling == MAXINT) {
					this->current = this->curnode().parent;
					if (this->current == MAXINT) return *this;
				}
				this->current = this->curnode().previousSibling;
			}
		} else {
			this->current = this->curnode().lastChild;
		}
		return *this;
	}

	template<typename It>
	PostorderIterator(const BaseIterator<T,It,reverse>& ii): Base(ii) {}
	PostorderIterator(const PostorderIterator& ii): Base(ii) {}
	PostorderIterator(PostorderIterator&& ii): Base(ii) {}
	PostorderIterator& operator=(const PostorderIterator& it) {
		Base::operator=(it);
		return *this;
	}
	PostorderIterator& operator=(PostorderIterator&& it) {
		Base::operator=(it);
		return *this;
	}
	virtual ~PostorderIterator() noexcept = default;
};
static_assert(std::is_copy_constructible<PostorderIterator<int,false>>::value, "");
static_assert(std::is_move_constructible<PostorderIterator<int,false>>::value, "");
static_assert(std::is_destructible<PostorderIterator<int,false>>::value, "");
static_assert(std::is_copy_constructible<PostorderIterator<int,true>>::value, "");
static_assert(std::is_move_constructible<PostorderIterator<int,true>>::value, "");
static_assert(std::is_destructible<PostorderIterator<int,true>>::value, "");

/**
 * Iterator class for iterations over all leaf elements.
 */
template<typename T, bool reverse = false>
struct LeafIterator:
	BaseIterator<T,LeafIterator<T,reverse>, reverse>,
	std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
{
	using Base = BaseIterator<T,LeafIterator<T,reverse>,reverse>;
	LeafIterator(const tree<T>* t): Base(t, MAXINT) {}
	LeafIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
	LeafIterator& next() {
		if (this->current == MAXINT) {
			this->current = this->mTree->begin_leaf().current;
		} else {
			PreorderIterator<T,false> it(*this);
			do {
				++it;
				if (it.current == MAXINT) break;
			} while (this->nodes()[it.current].firstChild != MAXINT);
			this->current = it.current;
		}
		return *this;
	}
	LeafIterator& previous() {
		if (this->current == MAXINT) {
			this->current = this->mTree->rbegin_leaf().current;
		} else {
			PreorderIterator<T,false> it(*this);
			do {
				--it;
				if (it.current == MAXINT) break;
			} while (this->nodes()[it.current].firstChild != MAXINT);
			this->current = it.current;
		}
		return *this;
	}

	template<typename It>
	LeafIterator(const BaseIterator<T,It,reverse>& ii): Base(ii) {}
	LeafIterator(const LeafIterator& ii): Base(ii) {}
	LeafIterator(LeafIterator&& ii): Base(ii) {}
	LeafIterator& operator=(const LeafIterator& it) {
		Base::operator=(it);
		return *this;
	}
	LeafIterator& operator=(LeafIterator&& it) {
		Base::operator=(it);
		return *this;
	}
	virtual ~LeafIterator() noexcept = default;
};
static_assert(std::is_copy_constructible<LeafIterator<int,false>>::value, "");
static_assert(std::is_move_constructible<LeafIterator<int,false>>::value, "");
static_assert(std::is_destructible<LeafIterator<int,false>>::value, "");
static_assert(std::is_copy_constructible<LeafIterator<int,true>>::value, "");
static_assert(std::is_move_constructible<LeafIterator<int,true>>::value, "");
static_assert(std::is_destructible<LeafIterator<int,true>>::value, "");

/**
 * Iterator class for iterations over all elements of a certain depth.
 */
template<typename T, bool reverse = false>
struct DepthIterator:
	BaseIterator<T,DepthIterator<T,reverse>,reverse>,
	std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
{
	using Base = BaseIterator<T,DepthIterator<T,reverse>,reverse>;
	std::size_t depth;
	DepthIterator(const tree<T>* t): Base(t, MAXINT), depth(0) {}
	DepthIterator(const tree<T>* t, std::size_t root, std::size_t _depth): Base(t, root), depth(_depth) {
		assert(!this->nodes().empty());
		if (reverse) {
			PostorderIterator<T,reverse> it(*this);
			while (it.current != MAXINT && it.depth() != _depth) ++it;
			this->current = it.current;
		} else {
			PreorderIterator<T,reverse> it(*this);
			while (it.current != MAXINT && it.depth() != _depth) ++it;
			this->current = it.current;
		}
	}
	DepthIterator& next() {
		if (this->current == MAXINT) {
			this->current = this->mTree->begin_depth(depth).current;
		} else if (this->curnode().nextSibling == MAXINT) {
			std::size_t target = this->curnode().depth;
			while (this->curnode().nextSibling == MAXINT) {
				this->current = this->curnode().parent;
				if (this->current == MAXINT) return *this;
			}
			PreorderIterator<T,reverse> it(this->mTree, this->curnode().nextSibling);
			for (; it.current != MAXINT; ++it) {
				if (it.depth() == target) break;
			}
			this->current = it.current;
		} else {
			this->current = this->curnode().nextSibling;
		}
		return *this;
	}
	DepthIterator& previous() {
		if (this->current == MAXINT) {
			this->current = this->mTree->rbegin_depth(depth).current;
		} else if (this->curnode().previousSibling == MAXINT) {
			std::size_t target = this->curnode().depth;
			while (this->curnode().previousSibling == MAXINT) {
				this->current = this->curnode().parent;
				if (this->current == MAXINT) return *this;
			}
			PostorderIterator<T,reverse> it(this->mTree, this->curnode().previousSibling);
			for (; it.current != MAXINT; ++it) {
				if (it.depth() == target) break;
			}
			this->current = it.current;
		} else {
			this->current = this->curnode().previousSibling;
		}
		return *this;
	}

	template<typename It>
	DepthIterator(const BaseIterator<T,It,reverse>& ii): Base(ii), depth(this->nodes()[ii.current].depth) {}
	DepthIterator(const DepthIterator& ii): Base(ii), depth(ii.depth) {}
	DepthIterator(DepthIterator&& ii): Base(ii), depth(ii.depth) {}
	DepthIterator& operator=(const DepthIterator& it) {
		Base::operator=(it);
		depth = it.depth;
		return *this;
	}
	DepthIterator& operator=(DepthIterator&& it) {
		Base::operator=(it);
		depth = it.depth;
		return *this;
	}
	virtual ~DepthIterator() noexcept = default;
};
static_assert(std::is_copy_constructible<DepthIterator<int,false>>::value, "");
static_assert(std::is_move_constructible<DepthIterator<int,false>>::value, "");
static_assert(std::is_destructible<DepthIterator<int,false>>::value, "");
static_assert(std::is_copy_constructible<DepthIterator<int,true>>::value, "");
static_assert(std::is_move_constructible<DepthIterator<int,true>>::value, "");
static_assert(std::is_destructible<DepthIterator<int,true>>::value, "");

/**
 * Iterator class for iterations over all children of a given element.
 */
template<typename T, bool reverse = false>
struct ChildrenIterator:
	BaseIterator<T,ChildrenIterator<T,reverse>,reverse>,
	std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
{
	using Base = BaseIterator<T,ChildrenIterator<T,reverse>,reverse>;
	std::size_t parent;
	ChildrenIterator(const tree<T>* t, std::size_t base, bool end = false): Base(t, base) {
		parent = base;
		assert(base != MAXINT);
		if (end) this->current = MAXINT;
		else if (this->curnode().firstChild == MAXINT) this->current = MAXINT;
		else {
			if (reverse) {
				this->current = this->curnode().lastChild;
			} else {
				this->current = this->curnode().firstChild;
			}
		}
	}
	ChildrenIterator& next() {
		if (this->current == MAXINT) {
			this->current = this->mTree->begin_children(PreorderIterator<T,false>(this->mTree, parent)).current;
		} else {
			this->current = this->curnode().nextSibling;
		}
		return *this;
	}
	ChildrenIterator& previous() {
		if (this->current == MAXINT) {
			this->current = this->mTree->rbegin_children(PreorderIterator<T,false>(this->mTree, parent)).current;
		} else {
			this->current = this->curnode().previousSibling;
		}
		return *this;
	}

	template<typename It>
	ChildrenIterator(const BaseIterator<T,It,reverse>& ii): Base(ii), parent(MAXINT) {
		if (this->mTree->is_valid(ii)) parent = this->nodes()[ii.current].parent;
	}
	ChildrenIterator(const ChildrenIterator& ii): Base(ii), parent(ii.parent) {}
	ChildrenIterator(ChildrenIterator&& ii): Base(ii), parent(ii.parent) {}
	ChildrenIterator& operator=(const ChildrenIterator& it) {
		Base::operator=(it);
		parent = it.parent;
		return *this;
	}
	ChildrenIterator& operator=(ChildrenIterator&& it) noexcept {
		Base::operator=(it);
		parent = it.parent;
		return *this;
	}
	virtual ~ChildrenIterator() noexcept = default;
};
static_assert(std::is_copy_constructible<ChildrenIterator<int,false>>::value, "");
static_assert(std::is_move_constructible<ChildrenIterator<int,false>>::value, "");
static_assert(std::is_destructible<ChildrenIterator<int,false>>::value, "");
static_assert(std::is_copy_constructible<ChildrenIterator<int,true>>::value, "");
static_assert(std::is_move_constructible<ChildrenIterator<int,true>>::value, "");
static_assert(std::is_destructible<ChildrenIterator<int,true>>::value, "");

/**
 * Iterator class for iterations from a given element to the root.
 */
template<typename T>
struct PathIterator:
	BaseIterator<T, PathIterator<T>,false>,
	std::iterator<std::forward_iterator_tag, T, std::size_t, T*, T&>
{
	using Base = BaseIterator<T, PathIterator<T>,false>;
	PathIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
	PathIterator& next() {
		if (this->current != MAXINT) {
			this->current = this->curnode().parent;
		}
		return *this;
	}

	template<typename It>
	PathIterator(const BaseIterator<T,It,false>& ii): Base(ii) {}
	PathIterator(const PathIterator& ii): Base(ii) {}
	PathIterator(PathIterator&& ii): Base(ii) {}
	PathIterator& operator=(const PathIterator& it) {
		Base::operator=(it);
		return *this;
	}
	PathIterator& operator=(PathIterator&& it) noexcept {
		Base::operator=(it);
		return *this;
	}
	virtual ~PathIterator() noexcept = default;
};
static_assert(std::is_copy_constructible<PathIterator<int>>::value, "");
static_assert(std::is_move_constructible<PathIterator<int>>::value, "");
static_assert(std::is_destructible<PathIterator<int>>::value, "");


}

/**
 * This class represents a tree.
 *
 * It tries to stick to the STL style as close as possible.
 */
template<typename T>
class tree {
public:
	using value_type = T;
	using Node = tree_detail::Node<T>;

	template<bool reverse>
	using PreorderIterator = tree_detail::PreorderIterator<T,reverse>;
	template<bool reverse>
	using PostorderIterator = tree_detail::PostorderIterator<T,reverse>;
	template<bool reverse>
	using LeafIterator = tree_detail::LeafIterator<T,reverse>;
	template<bool reverse>
	using DepthIterator = tree_detail::DepthIterator<T,reverse>;
	template<bool reverse>
	using ChildrenIterator = tree_detail::ChildrenIterator<T,reverse>;
	using PathIterator = tree_detail::PathIterator<T>;
private:
	template<typename TT, typename Iterator, bool reverse>
	friend struct tree_detail::BaseIterator;
	
	static constexpr std::size_t MAXINT = tree_detail::MAXINT;
	std::vector<Node> nodes;
	std::size_t emptyNodes = MAXINT;
public:

	using iterator = PreorderIterator<false>;

	tree() = default;
	tree(const tree& t) = default;
	tree(tree&& t) noexcept = default;
	tree& operator=(const tree& t) = default;
	tree& operator=(tree&& t) noexcept = default;

	void debug() const {
		std::cout << "emptyNodes: " << emptyNodes << std::endl;
		std::cout << this->nodes << std::endl;
	}

	iterator begin() const {
		return begin_preorder();
	}
	iterator end() const {
		return end_preorder();
	}
	iterator rbegin() const {
		return rbegin_preorder();
	}
	iterator rend() const {
		return rend_preorder();
	}

	PreorderIterator<false> begin_preorder() const {
		return PreorderIterator<false>(this, 0);
	}
	PreorderIterator<false> end_preorder() const {
		return PreorderIterator<false>(this);
	}
	PreorderIterator<true> rbegin_preorder() const {
		std::size_t cur = 0;
		while (nodes[cur].lastChild != MAXINT) cur = nodes[cur].lastChild;
		return PreorderIterator<true>(this, cur);
	}
	PreorderIterator<true> rend_preorder() const {
		return PreorderIterator<true>(this);
	}
	PostorderIterator<false> begin_postorder() const {
		std::size_t cur = 0;
		while (nodes[cur].firstChild != MAXINT) cur = nodes[cur].firstChild;
		return PostorderIterator<false>(this, cur);
	}
	PostorderIterator<false> end_postorder() const {
		return PostorderIterator<false>(this);
	}
	PostorderIterator<true> rbegin_postorder() const {
		return PostorderIterator<true>(this, 0);
	}
	PostorderIterator<true> rend_postorder() const {
		return PostorderIterator<true>(this);
	}
	LeafIterator<false> begin_leaf() const {
		std::size_t cur = 0;
		while (nodes[cur].firstChild != MAXINT) cur = nodes[cur].firstChild;
		return LeafIterator<false>(this, cur);
	}
	LeafIterator<false> end_leaf() const {
		return LeafIterator<false>(this);
	}
	LeafIterator<true> rbegin_leaf() const {
		std::size_t cur = 0;
		while (nodes[cur].lastChild != MAXINT) cur = nodes[cur].lastChild;
		return LeafIterator<true>(this, cur);
	}
	LeafIterator<true> rend_leaf() const {
		return LeafIterator<true>(this);
	}
	DepthIterator<false> begin_depth(std::size_t depth) const {
		return DepthIterator<false>(this, 0, depth);
	}
	DepthIterator<false> end_depth() const {
		return DepthIterator<false>(this);
	}
	DepthIterator<true> rbegin_depth(std::size_t depth) const {
		return DepthIterator<true>(this, 0, depth);
	}
	DepthIterator<true> rend_depth() const {
		return DepthIterator<true>(this);
	}
	template<typename Iterator>
	ChildrenIterator<false> begin_children(const Iterator& it) const {
		return ChildrenIterator<false>(this, it.current, false);
	}
	template<typename Iterator>
	ChildrenIterator<false> end_children(const Iterator& it) const {
		return ChildrenIterator<false>(this, it.current, true);
	}
	template<typename Iterator>
	ChildrenIterator<true> rbegin_children(const Iterator& it) const {
		return ChildrenIterator<true>(this, it.current, false);
	}
	template<typename Iterator>
	ChildrenIterator<true> rend_children(const Iterator& it) const {
		return ChildrenIterator<true>(this, it.current, true);
	}
	template<typename Iterator>
	PathIterator begin_path(const Iterator& it) const {
		return PathIterator(this, it.current);
	}
	PathIterator end_path() const {
		return PathIterator(this, MAXINT);
	}

	/**
	 * Retrieves the maximum depth of all elements.
	 * @return Maximum depth.
	 */
	std::size_t max_depth() const {
		std::size_t max = 0;
		for (auto it = begin_leaf(); it != end_leaf(); ++it) {
			if (it.depth() > max) max = it.depth();
		}
		return max;
	}
	template<typename Iterator>
	std::size_t max_depth(const Iterator& it) const {
		std::size_t max = 0;
		for (auto i = begin_children(it); i != end_children(it); ++i) {
			std::size_t d = max_depth(i);
			if (d + 1 > max) max = d + 1;
		}
		return max;
	}

	/**
	 * Check if the given element is a leaf.
	 * @param it Iterator.
	 * @return If `it` is a leaf.
	 */
	template<typename Iterator>
	bool is_leaf(const Iterator& it) const {
		return nodes[it.current].firstChild == MAXINT;
	}
	/**
	 * Check if the given element is a leftmost child.
	 * @param it Iterator.
	 * @return If `it` is a leftmost child.
	 */
	template<typename Iterator>
	bool is_leftmost(const Iterator& it) const {
		return nodes[it.current].previousSibling == MAXINT;
	}
	/**
	 * Check if the given element is a rightmost child.
	 * @param it Iterator.
	 * @return If `it` is a rightmost child.
	 */
	template<typename Iterator>
	bool is_rightmost(const Iterator& it) const {
		return nodes[it.current].nextSibling == MAXINT;
	}
	template<typename Iterator>
	bool is_valid(const Iterator& it) const {
		if (it.current >= nodes.size()) return false;
		return nodes[it.current].depth < MAXINT;
	}
	/**
	 * Retrieves the parent of an element.
	 * @param it Iterator.
	 * @return Parent of `it`.
	 */
	template<typename Iterator>
	Iterator get_parent(const Iterator& it) const {
		return Iterator(this, nodes[it.current].parent);
	}
	template<typename Iterator>
	Iterator left_sibling(const Iterator& it) const {
		assert(!is_leftmost(it));
		auto res = it;
		res.current = nodes[it.current].previousSibling;
		return res;
	}

	/**
	 * Sets the value of the root element.
	 * @param data Data.
	 * @return Iterator to the root.
	 */
	iterator setRoot(const T& data) {
		return setRoot(T(data));
	}
	iterator setRoot(T&& data) {
		if (nodes.empty()) nodes.emplace_back(0, std::move(data), MAXINT, 0);
		else nodes[0].data = data;
		return iterator(this, 0);
	}
	/**
	 * Clears the tree.
	 */
	void clear() {
		nodes.clear();
		emptyNodes = MAXINT;
	}
	/**
	 * Add the given data as last child of the root element.
	 * @param data Data.
	 * @return Iterator to inserted element.
	 */
	iterator append(const T& data) {
		if (nodes.empty()) setRoot(T());
		return append(iterator(this, 0), data);
	}

	/**
	 * Add the given data as last child of the given element.
	 * @param parent Parent element.
	 * @param data Data.
	 * @return Iterator to inserted element.
	 */
	template<typename Iterator>
	Iterator append(Iterator parent, const T& data) {
		std::size_t id = createNode(data, parent.current, nodes[parent.current].depth + 1);
		assert(isConsistent());
		return Iterator(this, id);
	}

	/**
	 * Insert element before the given position.
	 * @param position Position to insert before.
	 * @param data Element to insert.
	 * @return PreorderIterator to inserted element.
	 */
	template<typename Iterator>
	Iterator insert(Iterator position, const T& data) {
		std::size_t parent = nodes[position.current].parent;
		std::size_t newID = newNode(T(data), parent, nodes[position.current].depth);
		std::size_t prev = nodes[position.current].previousSibling;
		std::size_t next = position.current;
		nodes[newID].previousSibling = prev;
		nodes[newID].nextSibling = next;
		if (next != MAXINT) {
			nodes[next].previousSibling = newID;
		} else {
			nodes[parent].lastChild = newID;
		}
		if (prev != MAXINT) {
			nodes[prev].nextSibling = newID;
		} else {
			nodes[parent].firstChild = newID;
		}
		assert(isConsistent());
		return PreorderIterator<false>(this, newID);
	}

	/**
	 * Append another tree as last child of the root element.
	 * @param tree Tree.
	 * @return Iterator to root of inserted subtree.
	 */
	iterator append(tree&& tree) {
		if (nodes.empty()) std::swap(nodes, tree.nodes);
		return append(iterator(0), std::move(tree));
	}
	/**
	 * Append another tree as last child of the given element.
	 * @param position Element.
	 * @param tree Tree.
	 * @return Iterator to root of inserted subtree.
	 */
	template<typename Iterator>
	Iterator append(Iterator position, tree&& data) {
		Node* r = data.root;
		r->depth = position.depth() + 1;
		data.root = nullptr;
		r->parent = position.current;
		std::size_t id = position.current->children.size();
		if (id > 0) r->previousSibling = &(position.current->children.back());
		position.current->children.push_back(*r);
		if (id > 0) r->previousSibling->nextSibling = &(position.current->children.back());
		assert(isConsistent());
		return Iterator(&(position.current->children.back()));
	}

	template<typename Iterator>
	const Iterator& replace(const Iterator& position, const T& data) {
		nodes[position.current].data = data;
		return position;
	}

	/**
	 * Erase the element at the given position.
	 * Returns an iterator to the next position.
	 * @param position Element.
	 * @return Next element.
	 */
	template<typename Iterator>
	Iterator erase(Iterator position) {
		assert(this->isConsistent());
		std::size_t id = position.current;
		if (id == 0) {
			clear();
			++position;
			return position;
		}
		++position;
		if (nodes[id].nextSibling != MAXINT) {
			nodes[nodes[id].nextSibling].previousSibling = nodes[id].previousSibling;
		} else {
			nodes[nodes[id].parent].lastChild = nodes[id].previousSibling;
		}
		if (nodes[id].previousSibling != MAXINT) {
			nodes[nodes[id].previousSibling].nextSibling = nodes[id].nextSibling;
		} else {
			nodes[nodes[id].parent].firstChild = nodes[id].nextSibling;
		}
		eraseNode(id);
		assert(this->isConsistent());
		return position;
	}
	/**
	 * Erase all children of the given element.
	 * @param position Element.
	 */
	template<typename Iterator>
	void eraseChildren(const Iterator& position) {
		eraseChildren(position.current);
	}
private:
	std::size_t newNode(T&& data, std::size_t parent, std::size_t depth) {
		std::size_t newID = 0;
		if (emptyNodes == MAXINT) {
			nodes.emplace_back(nodes.size(), std::move(data), parent, depth);
			newID = nodes.size() - 1;
		} else {
			newID = emptyNodes;
			emptyNodes = nodes[emptyNodes].nextSibling;
			nodes[newID].data = data;
			nodes[newID].parent = parent;
			nodes[newID].depth = depth;
		}
		return newID;
	}
	std::size_t createNode(const T& data, std::size_t parent, std::size_t depth) {
		std::size_t res = newNode(T(data), parent, depth);
		nodes[res].nextSibling = MAXINT;
		if (parent != MAXINT) {
			if (nodes[parent].lastChild != MAXINT) {
				nodes[nodes[parent].lastChild].nextSibling = res;
				nodes[res].previousSibling = nodes[parent].lastChild;
				nodes[parent].lastChild = res;
			} else {
				nodes[parent].firstChild = res;
				nodes[parent].lastChild = res;
			}
		}
		return res;
	}
	void eraseChildren(std::size_t id) {
		if (nodes[id].firstChild == MAXINT) return;
		std::size_t cur = nodes[id].firstChild;
		while (cur != MAXINT) {
			std::size_t tmp = cur;
			cur = nodes[cur].nextSibling;
			eraseNode(tmp);
		}
		nodes[id].firstChild = MAXINT;
		nodes[id].lastChild = MAXINT;
	}
	void eraseNode(std::size_t id) {
		eraseChildren(id);
		nodes[id].nextSibling = emptyNodes;
		nodes[id].previousSibling = MAXINT;
		nodes[id].depth = MAXINT;
		emptyNodes = id;
	}

public:
	bool isConsistent() const {
		for (auto it = this->begin(); it != this->end(); ++it) {
			assert(isConsistent(it.current));
		}
		return true;
	}
	bool isConsistent(std::size_t node) const {
		assert(node != MAXINT);
		assert(node < nodes.size());
		if (nodes[node].firstChild != MAXINT) {
			std::size_t child = nodes[node].firstChild;
			while (nodes[child].nextSibling != MAXINT) {
				assert(nodes[child].parent == node);
				assert(nodes[nodes[child].nextSibling].previousSibling == child);
				child = nodes[child].nextSibling;
			}
			assert(child == nodes[node].lastChild);

			child = nodes[node].lastChild;
			while (nodes[child].previousSibling != MAXINT) {
				assert(nodes[child].parent == node);
				assert(nodes[nodes[child].previousSibling].nextSibling == child);
				child = nodes[child].previousSibling;
			}
			assert(child == nodes[node].firstChild);
		}
		return true;
	}
};


template<typename TT>
std::ostream& operator<<(std::ostream& os, const tree<TT>& tree) {
	for (auto it = tree.begin_preorder(); it != tree.end_preorder(); ++it) {
		os << std::string(it.depth(), '\t') << *it << std::endl;
	}
	return os;
}

template<typename T>
const std::size_t tree<T>::MAXINT;

}
