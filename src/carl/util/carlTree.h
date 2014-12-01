/**
 * @file Tree.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <list>
#include <stack>
#include <type_traits>
#include <vector>

namespace carl {

/**
 * This class represents a tree.
 *
 * It tries to stick to the STL style as close as possible.
 */
template<typename T>
class Tree {
private:
	struct Node {
		T data;
		Node* parent;
		Node* previousSibling;
		Node* nextSibling;
		std::list<Node> children;
		std::size_t depth;
		Node(const T& data, Node* const parent): data(data), parent(parent), previousSibling(nullptr), nextSibling(nullptr) {
			if (parent == nullptr) depth = 0;
			else depth = parent->depth + 1;
		}
		bool operator==(const Node& n) {
			return this == &n;
		}
		void updateDepth(std::size_t newDepth) {
			depth = newDepth;
			for (auto& c: children) c.updateDepth(newDepth + 1);
		}
	};
	
	Node* root;
	
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
		friend Tree;
	protected:
		Node* current;
		BaseIterator(Node* root): current(root) {}
	public:
		BaseIterator(const BaseIterator& ii): current(ii.current) {}
		BaseIterator(BaseIterator&& ii): current(ii.current) {}
		BaseIterator& operator=(const BaseIterator& ii) {
			this->current = ii.current;
			return *this;
		}
		BaseIterator& operator=(BaseIterator&& ii) {
			this->current = ii.current;
			return *this;
		}
		std::size_t depth() const {
			assert(current != nullptr);
			return current->depth;
		}
		T& operator*() {
			return this->current->data;
		}
		const T& operator*() const {
			return this->current->data;
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
		typename std::enable_if<reverse,I>::type& operator-() {
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
	
	template<typename Iterator>
	friend bool operator==(const Iterator& i1, const Iterator& i2) {
		return i1.current == i2.current;
	}
	template<typename Iterator>
	friend bool operator!=(const Iterator& i1, const Iterator& i2) {
		return i1.current != i2.current;
	}

	/**
	 * Iterator class for pre-order iterations over all elements.
	 */
	template<bool reverse = false>
	struct PreorderIterator: public BaseIterator<PreorderIterator<reverse>, reverse> {
		friend Tree;
	protected:
		typedef BaseIterator<PreorderIterator<reverse>, reverse> Base;
		PreorderIterator(): Base(nullptr) {}
		PreorderIterator(Node* root): Base(root) {}
		PreorderIterator& next() {
			if (this->current->children.empty()) {
				while (this->current->nextSibling == nullptr) {
					this->current = this->current->parent;
					if (this->current == nullptr) return *this;
				}
				this->current = this->current->nextSibling;
			} else {
				this->current = &this->current->children.front();
			}
			return *this;
		}
		PreorderIterator& previous() {
			if (this->current->previousSibling == nullptr) {
				this->current = this->current->parent;
			} else {
				this->current = this->current->previousSibling;
				while (!this->current->children.empty()) {
					this->current = &this->current->children.back();
				}
			}
			return *this;
		}
	public:
		PreorderIterator(const PreorderIterator& ii): Base(ii.current) {}
		PreorderIterator(PreorderIterator&& ii): Base(ii.current) {}
		virtual ~PreorderIterator() {}
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
	struct PostorderIterator: public BaseIterator<PostorderIterator<reverse>,reverse> {
		friend Tree;
	protected:
		typedef BaseIterator<PostorderIterator<reverse>,reverse> Base;
		PostorderIterator(): Base(nullptr) {}
		PostorderIterator(Node* root): Base(root) {}
		PostorderIterator& next() {
			if (this->current->nextSibling == nullptr) {
				this->current = this->current->parent;
			} else {
				this->current = this->current->nextSibling;
				while (!this->current->children.empty()) {
					this->current =	&this->current->children.front();
				}
			}
			return *this;
		}
		PostorderIterator& previous() {
			if (this->current->children.empty()) {
				if (this->current->previousSibling != nullptr) {
					this->current = this->current->previousSibling;
				} else {
					while (this->current->previousSibling == nullptr) {
						this->current = this->current->parent;
						if (this->current == nullptr) return *this;
					}
					this->current = this->current->previousSibling;
				}
			} else {
				this->current = &this->current->children.back();
			}
			return *this;
		}
	public:
		PostorderIterator(const PostorderIterator& ii): Base(ii.current) {}
		PostorderIterator(PostorderIterator&& ii): Base(ii.current) {}
		virtual ~PostorderIterator() {}
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
	struct LeafIterator: public PreorderIterator<false> {
		friend Tree;
	protected:
		LeafIterator(): PreorderIterator<false>() {}
		LeafIterator(Node* root): PreorderIterator<false>(root) {}
		LeafIterator& next() {
			do {
				PreorderIterator<false>::next();
				if (this->current == nullptr) break;
			} while (!this->current->children.empty());
			return *this;
		}
		LeafIterator& previous() {
			PreorderIterator<false> it(this->current);
			do {
				PreorderIterator<false>::previous();
				if (this->current == nullptr) break;
			} while (!this->current->children.empty());
			return *this;
		}
	public:
		LeafIterator(const LeafIterator& ii): PreorderIterator<false>(ii.current) {}
		LeafIterator(LeafIterator&& ii): PreorderIterator<false>(ii.current) {}
		virtual ~LeafIterator() {}
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
	struct DepthIterator: public BaseIterator<DepthIterator<reverse>,reverse> {
		friend Tree;
	protected:
		typedef BaseIterator<DepthIterator<reverse>,reverse> Base;
		DepthIterator(): Base(nullptr) {}
		DepthIterator(Node* root, std::size_t depth): Base(root) {
			assert(root != nullptr);
			if (reverse) {
				PostorderIterator<reverse> it(this->current);
				while (it.depth() != depth) ++it;
				this->current = it.current;
			} else {
				PreorderIterator<reverse> it(this->current);
				while (it.depth() != depth) ++it;
				this->current = it.current;
			}
		}
		DepthIterator& next() {
			if (this->current->nextSibling == nullptr) {
				std::size_t target = this->current->depth;
				while (this->current->nextSibling == nullptr) {
					this->current = this->current->parent;
					if (this->current == nullptr) return *this;
				}
				PreorderIterator<reverse> it(this->current->nextSibling);
				for (; it.current != nullptr; ++it) {
					if (it.depth() == target) break;
				}
				this->current = it.current;
			} else {
				this->current = this->current->nextSibling;
			}
			return *this;
		}
		DepthIterator& previous() {
			if (this->current->previousSibling == nullptr) {
				std::size_t target = this->current->depth;
				while (this->current->previousSibling == nullptr) {
					this->current = this->current->parent;
					if (this->current == nullptr) return *this;
				}
				PostorderIterator<reverse> it(this->current->previousSibling);
				for (; it.current != nullptr; ++it) {
					if (it.depth() == target) break;
				}
				this->current = it.current;
			} else {
				this->current = this->current->previousSibling;
			}
			return *this;
		}
	public:
		DepthIterator(const DepthIterator& ii): Base(ii.current) {}
		DepthIterator(DepthIterator&& ii): Base(ii.current) {}
		virtual ~DepthIterator() {}
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
	struct ChildrenIterator: public BaseIterator<ChildrenIterator<reverse>,reverse> {
		friend Tree;
	protected:
		typedef BaseIterator<ChildrenIterator<reverse>,reverse> Base;
		ChildrenIterator(): Base(nullptr) {}
		ChildrenIterator(Node* base): Base(base) {
			assert(base != nullptr);
			if (this->current->children.empty()) this->current = nullptr;
			else {
				if (reverse) {
					this->current = &this->current->children.back();
				} else {
					this->current = &this->current->children.front();
				}
			}
		}
		ChildrenIterator& next() {
			this->current = this->current->nextSibling;
			return *this;
		}
		ChildrenIterator& previous() {
			this->current = this->current->previousSibling;
			return *this;
		}
	public:
		ChildrenIterator(const ChildrenIterator& ii): Base(ii.current) {}
		ChildrenIterator(ChildrenIterator&& ii): Base(ii.current) {}
		virtual ~ChildrenIterator() {}
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
	struct PathIterator: public BaseIterator<PathIterator,false> {
		friend Tree;
	protected:
		typedef BaseIterator<PathIterator,false> Base;
		PathIterator(Node* root): Base(root) {}
		PathIterator& next() {
			if (this->current != nullptr) {
				this->current = this->current->parent;
			}
			return *this;
		}
	public:
		PathIterator(const PathIterator& ii): Base(ii.current) {}
		PathIterator(PathIterator&& ii): Base(ii.current) {}
		virtual ~PathIterator() {}
	};
	static_assert(std::is_copy_constructible<PathIterator>::value, "");
	static_assert(std::is_move_constructible<PathIterator>::value, "");
	static_assert(std::is_destructible<PathIterator>::value, "");

	Tree() {
		root = nullptr;
	}

	PreorderIterator<false> begin_preorder() const {
		return PreorderIterator<false>(root);
	}
	PreorderIterator<false> end_preorder() const {
		return PreorderIterator<false>();
	}
	PreorderIterator<true> rbegin_preorder() const {
		Node* cur = root;
		while (!cur->children.empty()) cur = &(cur->children.back());
		return PreorderIterator<true>(cur);
	}
	PreorderIterator<true> rend_preorder() const {
		return PreorderIterator<true>();
	}
	PostorderIterator<false> begin_postorder() const {
		Node* cur = root;
		while (!cur->children.empty()) cur = &(cur->children.front());
		return PostorderIterator<false>(cur);
	}
	PostorderIterator<false> end_postorder() const {
		return PostorderIterator<false>();
	}
	PostorderIterator<true> rbegin_postorder() const {
		return PostorderIterator<true>(root);
	}
	PostorderIterator<true> rend_postorder() const {
		return PostorderIterator<true>();
	}
	LeafIterator<false> begin_leaf() const {
		Node* cur = root;
		while (!cur->children.empty()) cur = &(cur->children.front());
		return LeafIterator<false>(cur);
	}
	LeafIterator<false> end_leaf() const {
		return LeafIterator<false>();
	}
	LeafIterator<true> rbegin_leaf() const {
		Node* cur = root;
		while (!cur->children.empty()) cur = &(cur->children.back());
		return LeafIterator<true>(cur);
	}
	LeafIterator<true> rend_leaf() const {
		return LeafIterator<true>();
	}
	DepthIterator<false> begin_depth(std::size_t depth) const {
		return DepthIterator<false>(root, depth);
	}
	DepthIterator<false> end_depth() const {
		return DepthIterator<false>();
	}
	DepthIterator<true> rbegin_depth(std::size_t depth) const {
		return DepthIterator<true>(root, depth);
	}
	DepthIterator<true> rend_depth() const {
		return DepthIterator<true>();
	}
	template<typename Iterator>
	ChildrenIterator<false> begin_children(const Iterator& it) const {
		return ChildrenIterator<false>(it.current);
	}
	ChildrenIterator<false> end_children() const {
		return ChildrenIterator<false>();
	}
	template<typename Iterator>
	ChildrenIterator<true> rbegin_children(const Iterator& it) const {
		return ChildrenIterator<true>(it.current);
	}
	ChildrenIterator<true> rend_children() const {
		return ChildrenIterator<true>();
	}
	template<typename Iterator>
	PathIterator begin_path(const Iterator& it) const {
		return PathIterator(it.current);
	}
	PathIterator end_path() const {
		return PathIterator(nullptr);
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

	/**
	 * Sets the value of the root element.
     * @param data Data.
     * @return Iterator to the root.
     */
	PreorderIterator<> setRoot(const T& data) {
		if (root == nullptr) root = new Node(data, nullptr);
		else root->data = data;
		return PreorderIterator<>(root);
	}
	/**
	 * Clears the tree.
     */
	void clear() {
		delete root;
		root = nullptr;
	}
	/**
	 * Add the given data as last child of the root element.
     * @param data Data.
     * @return Iterator to inserted element.
     */
	PreorderIterator<> insert(const T& data) {
		if (root == nullptr) setRoot(T());
		return insert(PreorderIterator<>(root), data);
	}
	/**
	 * Add the given data as last child of the given element.
     * @param position Element.
     * @param data Data.
     * @return Iterator to inserted element.
     */
	template<typename Iterator>
	Iterator insert(Iterator position, const T& data) {
		Node n(data, position.current);
		std::size_t id = position.current->children.size();
		if (id > 0) n.previousSibling = &(position.current->children.back());
		position.current->children.push_back(n);
		if (id > 0) n.previousSibling->nextSibling = &(position.current->children.back());
		return Iterator(&(position.current->children.back()));
	}
	/**
	 * Append another tree as last child of the root element.
     * @param tree Tree.
     * @return Iterator to root of inserted subtree.
     */
	PreorderIterator<> append(Tree&& tree) {
		if (root == nullptr) std::swap(root, tree.root);
		return append(PreorderIterator<>(root), std::move(tree));
	}
	/**
	 * Append another tree as last child of the given element.
	 * @param position Element.
     * @param tree Tree.
     * @return Iterator to root of inserted subtree.
     */
	template<typename Iterator>
	Iterator append(Iterator position, Tree&& data) {
		Node* r = data.root;
		r->updateDepth(position.depth() + 1);
		data.root = nullptr;
		r->parent = position.current;
		std::size_t id = position.current->children.size();
		if (id > 0) r->previousSibling = &(position.current->children.back());
		position.current->children.push_back(*r);
		if (id > 0) r->previousSibling->nextSibling = &(position.current->children.back());
		return Iterator(&(position.current->children.back()));
	}

	/**
	 * Erase the element at the given position.
	 * Returns an iterator to the next position.
     * @param position Element.
     * @return Next element.
     */
	template<typename Iterator>
	Iterator erase(Iterator position) {
		eraseChildren(position);
		Node* n = position.current;
		++position;
		if (n->parent == nullptr) clear();
		else {
			if (n->nextSibling != nullptr) {
				n->nextSibling->previousSibling = n->previousSibling;
			}
			if (n->previousSibling != nullptr) {
				n->previousSibling->nextSibling = n->nextSibling;
			}
			n->parent->children.remove(*n);
		}
		return position;
	}
	/**
	 * Erase all children of the given element.
     * @param position Element.
     */
	template<typename Iterator>
	void eraseChildren(const Iterator& position) {
		position.current->children.clear();
	}

	template<typename TT>
	friend std::ostream& operator<<(std::ostream& os, const Tree<TT>& tree) {
		for (auto it = tree.begin_preorder(); it != tree.end_preorder(); it++) {
			os << std::string(it.depth(), '\t') << *it << std::endl;
		}
		return os;
	}
};



}