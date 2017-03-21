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

#include "../io/streamingOperators.h"

namespace carl {

/**
 * This class represents a tree.
 *
 * It tries to stick to the STL style as close as possible.
 */
template<typename T>
class tree {
private:
#ifdef __VS
	//Warning: might lead to problem when using 64bit
	static const std::size_t MAXINT = UINT_MAX;
#else
	static const std::size_t MAXINT = std::numeric_limits<std::size_t>::max();
#endif
public:
	using value_type = T;
	struct Node {
		std::size_t id;
		mutable T data;
		std::size_t parent;
		std::size_t previousSibling = MAXINT;
		std::size_t nextSibling = MAXINT;
		std::size_t firstChild = MAXINT;
		std::size_t lastChild = MAXINT;
		std::size_t depth = MAXINT;
		Node(std::size_t _id, const T& _data, std::size_t _parent, std::size_t _depth):
			id(_id), data(_data), parent(_parent), depth(_depth)
		{
		}
		bool operator==(const Node& n) {
			return this == &n;
		}
		void updateDepth(std::size_t newDepth) {
			depth = newDepth;
			//for (auto& c: children) nodes[c].updateDepth(newDepth + 1);
		}
	};
	friend std::ostream& operator<<(std::ostream& os, const Node& n) {
		int id = (int)n.id;
		int parent = (n.parent == MAXINT ? -1 : (int)n.parent);
		int firstChild = (n.firstChild == MAXINT ? -1 : (int)n.firstChild);
		int lastChild = (n.lastChild == MAXINT ? -1 : (int)n.lastChild);
		int previousSibling = (n.previousSibling == MAXINT ? -1 : (int)n.previousSibling);
		int nextSibling = (n.nextSibling == MAXINT ? -1 : (int)n.nextSibling);
		os << "(" << n.data << " @ " << id << ", " << parent << ", " << firstChild << ":" << lastChild << ", " << previousSibling << " <-> " << nextSibling << ")\n";

		return os;
	}
	std::vector<Node> nodes;
	std::size_t emptyNodes = MAXINT;
protected:
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
	template<typename Iterator, bool reverse>
	struct BaseIterator {
		friend tree;
	protected:
		const tree<T>* mTree;
		BaseIterator(const tree<T>* t, std::size_t root): mTree(t), current(root) {}
	public:
		std::size_t current;
		BaseIterator(const BaseIterator& ii) = default;
		BaseIterator(BaseIterator&& ii) noexcept = default;
		template<typename It, bool r>
		BaseIterator(const BaseIterator<It,r>& ii): mTree(ii.mTree), current(ii.current) {}
		BaseIterator& operator=(const BaseIterator& ii) = default;
		BaseIterator& operator=(BaseIterator&& ii) noexcept = default;
		std::size_t depth() const {
			assert(current != MAXINT);
			return mTree->nodes[current].depth;
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
		T& operator*() {
			assert(current != MAXINT);
			return mTree->nodes[current].data;
		}
		const T& operator*() const {
			assert(current != MAXINT);
			return mTree->nodes[current].data;
		}
		T* operator->() {
			assert(current != MAXINT);
			return &(mTree->nodes[current].data);
		}
		T const * operator->() const {
			assert(current != MAXINT);
			return &(mTree->nodes[current].data);
		}

		template<typename I = Iterator>
		typename std::enable_if<!reverse,I>::type& operator++() {
			return static_cast<Iterator*>(this)->next();
		}
		template<typename I = Iterator>
		typename std::enable_if<reverse,I>::type& operator++() {
			return static_cast<Iterator*>(this)->previous();
		}
		template<typename I = Iterator>
		typename std::enable_if<!reverse,I>::type operator++(int) {
			return static_cast<Iterator*>(this)->next();
		}
		template<typename I = Iterator>
		typename std::enable_if<reverse,I>::type operator++(int) {
			return static_cast<Iterator*>(this)->previous();
		}
		template<typename I = Iterator>
		typename std::enable_if<!reverse,I>::type& operator--() {
			return static_cast<Iterator*>(this)->previous();
		}
		template<typename I = Iterator>
		typename std::enable_if<reverse,I>::type& operator--() {
			return static_cast<Iterator*>(this)->next();
		}
		template<typename I = Iterator>
		typename std::enable_if<!reverse,I>::type operator--(int) {
			return static_cast<Iterator*>(this)->previous();
		}
		template<typename I = Iterator>
		typename std::enable_if<reverse,I>::type operator--(int) {
			return static_cast<Iterator*>(this)->next();
		}
	};
public:
	template<typename I, bool r>
	friend bool operator==(const BaseIterator<I,r>& i1, const BaseIterator<I,r>& i2) {
		return i1.current == i2.current;
	}
	template<typename I, bool r>
	friend bool operator!=(const BaseIterator<I,r>& i1, const BaseIterator<I,r>& i2) {
		return i1.current != i2.current;
	}

	/**
	 * Iterator class for pre-order iterations over all elements.
	 */
	template<bool reverse = false>
	struct PreorderIterator:
		public BaseIterator<PreorderIterator<reverse>, reverse>,
		public std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
	{
		friend tree;
	protected:
		using Base = BaseIterator<PreorderIterator<reverse>, reverse>;
		PreorderIterator(const tree<T>* t): Base(t, MAXINT) {}
		PreorderIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
		PreorderIterator& next() {
			if (this->current == MAXINT) {
				this->current = this->mTree->begin_preorder().current;
			} else if (this->mTree->nodes[this->current].firstChild == MAXINT) {
				while (this->mTree->nodes[this->current].nextSibling == MAXINT) {
					this->current = this->mTree->nodes[this->current].parent;
					if (this->current == MAXINT) return *this;
				}
				this->current = this->mTree->nodes[this->current].nextSibling;
			} else {
				this->current = this->mTree->nodes[this->current].firstChild;
			}
			return *this;
		}
		PreorderIterator& previous() {
			if (this->current == MAXINT) {
				this->current = this->mTree->rbegin_preorder().current;
			} else if (this->mTree->nodes[this->current].previousSibling == MAXINT) {
				this->current = this->mTree->nodes[this->current].parent;
			} else {
				this->current = this->mTree->nodes[this->current].previousSibling;
				while (this->mTree->nodes[this->current].firstChild != MAXINT) {
					this->current = this->mTree->nodes[this->current].lastChild;
				}
			}
			return *this;
		}
	public:
		template<typename It>
		PreorderIterator(const BaseIterator<It,reverse>& ii): Base(ii) {}
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
		virtual ~PreorderIterator() = default;
		
		PreorderIterator& skipChildren() {
			assert(this->current != MAXINT);
			while (this->mTree->nodes[this->current].nextSibling == MAXINT) {
				this->current = this->mTree->nodes[this->current].parent;
				if (this->current == MAXINT) return *this;
			}
			this->current = this->mTree->nodes[this->current].nextSibling;
			return *this;
		}
	};
	static_assert(std::is_copy_constructible<PreorderIterator<false>>::value, "");
	static_assert(std::is_move_constructible<PreorderIterator<false>>::value, "");
	static_assert(std::is_destructible<PreorderIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<PreorderIterator<true>>::value, "");
	static_assert(std::is_move_constructible<PreorderIterator<true>>::value, "");
	static_assert(std::is_destructible<PreorderIterator<true>>::value, "");

	/**
	 * Iterator class for post-order iterations over all elements.
	 */
	template<bool reverse = false>
	struct PostorderIterator:
		public BaseIterator<PostorderIterator<reverse>,reverse>,
		public std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
	{
		friend tree;
	protected:
		using Base = BaseIterator<PostorderIterator<reverse>,reverse>;
		PostorderIterator(const tree<T>* t): Base(t, MAXINT) {}
		PostorderIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
		PostorderIterator& next() {
			if (this->current == MAXINT) {
				this->current = this->mTree->begin_postorder().current;
			} else  if (this->mTree->nodes[this->current].nextSibling == MAXINT) {
				this->current = this->mTree->nodes[this->current].parent;
			} else {
				this->current = this->mTree->nodes[this->current].nextSibling;
				while (this->mTree->nodes[this->current].firstChild != MAXINT) {
					this->current =	this->mTree->nodes[this->current].firstChild;
				}
			}
			return *this;
		}
		PostorderIterator& previous() {
			if (this->current == MAXINT) {
				this->current = this->mTree->rbegin_postorder().current;
			} else if (this->mTree->nodes[this->current].firstChild == MAXINT) {
				if (this->mTree->nodes[this->current].previousSibling != MAXINT) {
					this->current = this->mTree->nodes[this->current].previousSibling;
				} else {
					while (this->mTree->nodes[this->current].previousSibling == MAXINT) {
						this->current = this->mTree->nodes[this->current].parent;
						if (this->current == MAXINT) return *this;
					}
					this->current = this->mTree->nodes[this->current].previousSibling;
				}
			} else {
				this->current = this->mTree->nodes[this->current].lastChild;
			}
			return *this;
		}
	public:
		template<typename It>
		PostorderIterator(const BaseIterator<It,reverse>& ii): Base(ii) {}
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
		virtual ~PostorderIterator() = default;
	};
	static_assert(std::is_copy_constructible<PostorderIterator<false>>::value, "");
	static_assert(std::is_move_constructible<PostorderIterator<false>>::value, "");
	static_assert(std::is_destructible<PostorderIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<PostorderIterator<true>>::value, "");
	static_assert(std::is_move_constructible<PostorderIterator<true>>::value, "");
	static_assert(std::is_destructible<PostorderIterator<true>>::value, "");

	/**
	 * Iterator class for iterations over all leaf elements.
	 */
	template<bool reverse = false>
	struct LeafIterator:
		public BaseIterator<LeafIterator<reverse>, reverse>,
		public std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
	{
		friend tree;
	protected:
		using Base = BaseIterator<LeafIterator<reverse>,reverse>;
		LeafIterator(const tree<T>* t): Base(t, MAXINT) {}
		LeafIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
		LeafIterator& next() {
			if (this->current == MAXINT) {
				this->current = this->mTree->begin_leaf().current;
			} else {
				PreorderIterator<false> it(this->mTree, this->current);
				do {
					it++;
					if (it.current == MAXINT) break;
				} while (this->mTree->nodes[it.current].firstChild != MAXINT);
				this->current = it.current;
			}
			return *this;
		}
		LeafIterator& previous() {
			if (this->current == MAXINT) {
				this->current = this->mTree->rbegin_leaf().current;
			} else {
				PreorderIterator<false> it(this->mTree, this->current);
				do {
					--it;
					if (it.current == MAXINT) break;
				} while (this->mTree->nodes[it.current].firstChild != MAXINT);
				this->current = it.current;
			}
			return *this;
		}
	public:
		template<typename It>
		LeafIterator(const BaseIterator<It,reverse>& ii): Base(ii) {}
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
		virtual ~LeafIterator() = default;
	};
	static_assert(std::is_copy_constructible<LeafIterator<false>>::value, "");
	static_assert(std::is_move_constructible<LeafIterator<false>>::value, "");
	static_assert(std::is_destructible<LeafIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<LeafIterator<true>>::value, "");
	static_assert(std::is_move_constructible<LeafIterator<true>>::value, "");
	static_assert(std::is_destructible<LeafIterator<true>>::value, "");

	/**
	 * Iterator class for iterations over all elements of a certain depth.
	 */
	template<bool reverse = false>
	struct DepthIterator:
		public BaseIterator<DepthIterator<reverse>,reverse>,
		public std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
	{
		friend tree;
	protected:
		using Base = BaseIterator<DepthIterator<reverse>,reverse>;
		std::size_t depth;
		DepthIterator(const tree<T>* t): Base(t, MAXINT), depth(0) {}
		DepthIterator(const tree<T>* t, std::size_t root, std::size_t _depth): Base(t, root), depth(_depth) {
			assert(!this->mTree->nodes.empty());
			if (reverse) {
				PostorderIterator<reverse> it(this->mTree, this->current);
				while (it.current != MAXINT && it.depth() != _depth) ++it;
				this->current = it.current;
			} else {
				PreorderIterator<reverse> it(this->mTree, this->current);
				while (it.current != MAXINT && it.depth() != _depth) ++it;
				this->current = it.current;
			}
		}
		DepthIterator& next() {
			if (this->current == MAXINT) {
				this->current = this->mTree->begin_depth(depth).current;
			} else if (this->mTree->nodes[this->current].nextSibling == MAXINT) {
				std::size_t target = this->mTree->nodes[this->current].depth;
				while (this->mTree->nodes[this->current].nextSibling == MAXINT) {
					this->current = this->mTree->nodes[this->current].parent;
					if (this->current == MAXINT) return *this;
				}
				PreorderIterator<reverse> it(this->mTree, this->mTree->nodes[this->current].nextSibling);
				for (; it.current != MAXINT; it++) {
					if (it.depth() == target) break;
				}
				this->current = it.current;
			} else {
				this->current = this->mTree->nodes[this->current].nextSibling;
			}
			return *this;
		}
		DepthIterator& previous() {
			if (this->current == MAXINT) {
				this->current = this->mTree->rbegin_depth(depth).current;
			} else if (this->mTree->nodes[this->current].previousSibling == MAXINT) {
				std::size_t target = this->mTree->nodes[this->current].depth;
				while (this->mTree->nodes[this->current].previousSibling == MAXINT) {
					this->current = this->mTree->nodes[this->current].parent;
					if (this->current == MAXINT) return *this;
				}
				PostorderIterator<reverse> it(this->mTree, this->mTree->nodes[this->current].previousSibling);
				for (; it.current != MAXINT; ++it) {
					if (it.depth() == target) break;
				}
				this->current = it.current;
			} else {
				this->current = this->mTree->nodes[this->current].previousSibling;
			}
			return *this;
		}
	public:
		template<typename It>
		DepthIterator(const BaseIterator<It,reverse>& ii): Base(ii), depth(nodes[ii.current].depth) {}
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
		virtual ~DepthIterator() = default;
	};
	static_assert(std::is_copy_constructible<DepthIterator<false>>::value, "");
	static_assert(std::is_move_constructible<DepthIterator<false>>::value, "");
	static_assert(std::is_destructible<DepthIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<DepthIterator<true>>::value, "");
	static_assert(std::is_move_constructible<DepthIterator<true>>::value, "");
	static_assert(std::is_destructible<DepthIterator<true>>::value, "");

	/**
	 * Iterator class for iterations over all children of a given element.
	 */
	template<bool reverse = false>
	struct ChildrenIterator:
		public BaseIterator<ChildrenIterator<reverse>,reverse>,
		public std::iterator<std::bidirectional_iterator_tag, T, std::size_t, T*, T&>
	{
		friend tree;
	protected:
		using Base = BaseIterator<ChildrenIterator<reverse>,reverse>;
		std::size_t parent;
		ChildrenIterator(const tree<T>* t, std::size_t base, bool end = false): Base(t, base) {
			parent = base;
			assert(base != MAXINT);
			if (end) this->current = MAXINT;
			else if (this->mTree->nodes[this->current].firstChild == MAXINT) this->current = MAXINT;
			else {
				if (reverse) {
					this->current = this->mTree->nodes[this->current].lastChild;
				} else {
					this->current = this->mTree->nodes[this->current].firstChild;
				}
			}
		}
		ChildrenIterator& next() {
			if (this->current == MAXINT) {
				this->current = this->mTree->begin_children(PreorderIterator<false>(this->mTree, parent)).current;
			} else {
				this->current = this->mTree->nodes[this->current].nextSibling;
			}
			return *this;
		}
		ChildrenIterator& previous() {
			if (this->current == MAXINT) {
				this->current = this->mTree->rbegin_children(PreorderIterator<false>(this->mTree, parent)).current;
			} else {
				this->current = this->mTree->nodes[this->current].previousSibling;
			}
			return *this;
		}
	public:
		template<typename It>
		ChildrenIterator(const BaseIterator<It,reverse>& ii): Base(ii), parent(MAXINT) {
			if (this->mTree->is_valid(ii)) parent = this->mTree->nodes[ii.current].parent;
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
		virtual ~ChildrenIterator() = default;
	};
	static_assert(std::is_copy_constructible<ChildrenIterator<false>>::value, "");
	static_assert(std::is_move_constructible<ChildrenIterator<false>>::value, "");
	static_assert(std::is_destructible<ChildrenIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<ChildrenIterator<true>>::value, "");
	static_assert(std::is_move_constructible<ChildrenIterator<true>>::value, "");
	static_assert(std::is_destructible<ChildrenIterator<true>>::value, "");

	/**
	 * Iterator class for iterations from a given element to the root.
	 */
	struct PathIterator:
		public BaseIterator<PathIterator,false>,
		public std::iterator<std::forward_iterator_tag, T, std::size_t, T*, T&>
	{
		friend tree;
	protected:
		using Base = BaseIterator<PathIterator,false>;
		PathIterator(const tree<T>* t, std::size_t root): Base(t, root) {}
		PathIterator& next() {
			if (this->current != MAXINT) {
				this->current = this->mTree->nodes[this->current].parent;
			}
			return *this;
		}
	public:
		template<typename It>
		PathIterator(const BaseIterator<It,false>& ii): Base(ii) {}
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
		virtual ~PathIterator() = default;
	};
	static_assert(std::is_copy_constructible<PathIterator>::value, "");
	static_assert(std::is_move_constructible<PathIterator>::value, "");
	static_assert(std::is_destructible<PathIterator>::value, "");

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
		for (auto it = begin_leaf(); it != end_leaf(); it++) {
			if (it.depth() > max) max = it.depth();
		}
		return max;
	}
	template<typename Iterator>
	std::size_t max_depth(const Iterator& it) const {
		std::size_t max = 0;
		for (auto i = begin_children(it); i != end_children(it); i++) {
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
	PreorderIterator<> setRoot(const T& data) {
		if (nodes.empty()) nodes.emplace_back(0, data, MAXINT, 0);
		else nodes[0].data = data;
		return PreorderIterator<>(this, 0);
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
	PreorderIterator<> append(const T& data) {
		if (nodes.empty()) setRoot(T());
		return append(PreorderIterator<>(this, 0), data);
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
		std::size_t newID = newNode(data, parent, nodes[position.current].depth);
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
	PreorderIterator<> append(tree&& tree) {
		if (nodes.empty()) std::swap(nodes, tree.nodes);
		return append(PreorderIterator<>(0), std::move(tree));
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
		r->updateDepth(position.depth() + 1);
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
			position++;
			return position;
		}
		position++;
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
	std::size_t newNode(const T& data, std::size_t parent, std::size_t depth) {
		std::size_t newID = 0;
		if (emptyNodes == MAXINT) {
			nodes.emplace_back(nodes.size(), data, parent, depth);
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
		std::size_t res = newNode(data, parent, depth);
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
		for (auto it = this->begin(); it != this->end(); it++) {
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
	for (auto it = tree.begin_preorder(); it != tree.end_preorder(); it++) {
		os << std::string(it.depth(), '\t') << *it << std::endl;
	}
	return os;
}

template<typename T>
const std::size_t tree<T>::MAXINT;

}
