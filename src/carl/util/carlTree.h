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
	};
	
	Node* root;
	
public:

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
		bool operator==(const BaseIterator& i) {
			return current == i.current;
		}
		bool operator!=(const BaseIterator& i) {
			return current != i.current;
		}
	};
	
	template<bool reverse = false>
	struct PreorderIterator: public BaseIterator {
		friend Tree;
	protected:
		PreorderIterator(): BaseIterator(nullptr) {}
		PreorderIterator(Node* root): BaseIterator(root) {}
	public:
		PreorderIterator(const PreorderIterator& ii): BaseIterator(ii.current) {}
		PreorderIterator(PreorderIterator&& ii): BaseIterator(ii.current) {}
		virtual ~PreorderIterator() {}
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
		PreorderIterator operator++() {
			return reverse ? previous() : next();
		}
		PreorderIterator operator++(int) {
			return reverse ? previous() : next();
		}
		PreorderIterator operator--() {
			return reverse ? next() : previous();
		}
		PreorderIterator operator--(int) {
			return reverse ? next() : previous();
		}
	};
	static_assert(std::is_copy_constructible<PreorderIterator<false>>::value, "");
	static_assert(std::is_move_constructible<PreorderIterator<false>>::value, "");
	static_assert(std::is_destructible<PreorderIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<PreorderIterator<true>>::value, "");
	static_assert(std::is_move_constructible<PreorderIterator<true>>::value, "");
	static_assert(std::is_destructible<PreorderIterator<true>>::value, "");
	
	template<bool reverse = false>
	struct PostorderIterator: public BaseIterator {
		friend Tree;
	protected:
		PostorderIterator(): BaseIterator(nullptr) {}
		PostorderIterator(Node* root): BaseIterator(root) {}
	public:
		PostorderIterator(const PostorderIterator& ii): BaseIterator(ii.current) {}
		PostorderIterator(PostorderIterator&& ii): BaseIterator(ii.current) {}
		virtual ~PostorderIterator() {}
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
		PostorderIterator operator++() {
			return reverse ? previous() : next();
		}
		PostorderIterator operator++(int) {
			return reverse ? previous() : next();
		}
		PostorderIterator operator--() {
			return reverse ? next() : previous();
		}
		PostorderIterator operator--(int) {
			return reverse ? next() : previous();
		}
	};
	static_assert(std::is_copy_constructible<PostorderIterator<false>>::value, "");
	static_assert(std::is_move_constructible<PostorderIterator<false>>::value, "");
	static_assert(std::is_destructible<PostorderIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<PostorderIterator<true>>::value, "");
	static_assert(std::is_move_constructible<PostorderIterator<true>>::value, "");
	static_assert(std::is_destructible<PostorderIterator<true>>::value, "");
	
	template<bool reverse = false>
	struct LeafIterator: public PreorderIterator<false> {
		friend Tree;
	protected:
		LeafIterator(): PreorderIterator<false>() {}
		LeafIterator(Node* root): PreorderIterator<false>(root) {}
	public:
		LeafIterator(const LeafIterator& ii): PreorderIterator<false>(ii.current) {}
		LeafIterator(LeafIterator&& ii): PreorderIterator<false>(ii.current) {}
		virtual ~LeafIterator() {}
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
		LeafIterator operator++() {
			return reverse ? previous() : next();
		}
		LeafIterator operator++(int) {
			return reverse ? previous() : next();
		}
		LeafIterator operator--() {
			return reverse ? next() : previous();
		}
		LeafIterator operator--(int) {
			return reverse ? next() : previous();
		}
	};
	static_assert(std::is_copy_constructible<LeafIterator<false>>::value, "");
	static_assert(std::is_move_constructible<LeafIterator<false>>::value, "");
	static_assert(std::is_destructible<LeafIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<LeafIterator<true>>::value, "");
	static_assert(std::is_move_constructible<LeafIterator<true>>::value, "");
	static_assert(std::is_destructible<LeafIterator<true>>::value, "");
	
	template<bool reverse = false>
	struct DepthIterator: public BaseIterator {
		friend Tree;
	protected:
		DepthIterator(): BaseIterator(nullptr) {}
		DepthIterator(Node* root, std::size_t depth): BaseIterator(root) {
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
	public:
		DepthIterator(const DepthIterator& ii): BaseIterator(ii.current) {}
		DepthIterator(DepthIterator&& ii): BaseIterator(ii.current) {}
		virtual ~DepthIterator() {}
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
		DepthIterator operator++() {
			return reverse ? previous() : next();
		}
		DepthIterator operator++(int) {
			return reverse ? previous() : next();
		}
		DepthIterator operator--() {
			return reverse ? next() : previous();
		}
		DepthIterator operator--(int) {
			return reverse ? next() : previous();
		}
	};
	static_assert(std::is_copy_constructible<DepthIterator<false>>::value, "");
	static_assert(std::is_move_constructible<DepthIterator<false>>::value, "");
	static_assert(std::is_destructible<DepthIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<DepthIterator<true>>::value, "");
	static_assert(std::is_move_constructible<DepthIterator<true>>::value, "");
	static_assert(std::is_destructible<DepthIterator<true>>::value, "");
	
	template<bool reverse = false>
	struct ChildrenIterator: public BaseIterator {
		friend Tree;
	protected:
		ChildrenIterator(): BaseIterator(nullptr) {}
		ChildrenIterator(Node* base): BaseIterator(base) {
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
	public:
		ChildrenIterator(const ChildrenIterator& ii): BaseIterator(ii.current) {}
		ChildrenIterator(ChildrenIterator&& ii): BaseIterator(ii.current) {}
		virtual ~ChildrenIterator() {}
		ChildrenIterator& next() {
			this->current = this->current->nextSibling;
			return *this;
		}
		ChildrenIterator& previous() {
			this->current = this->current->previousSibling;
			return *this;
		}
		ChildrenIterator operator++() {
			return reverse ? previous() : next();
		}
		ChildrenIterator operator++(int) {
			return reverse ? previous() : next();
		}
		ChildrenIterator operator--() {
			return reverse ? next() : previous();
		}
		ChildrenIterator operator--(int) {
			return reverse ? next() : previous();
		}
	};
	static_assert(std::is_copy_constructible<ChildrenIterator<false>>::value, "");
	static_assert(std::is_move_constructible<ChildrenIterator<false>>::value, "");
	static_assert(std::is_destructible<ChildrenIterator<false>>::value, "");
	static_assert(std::is_copy_constructible<ChildrenIterator<true>>::value, "");
	static_assert(std::is_move_constructible<ChildrenIterator<true>>::value, "");
	static_assert(std::is_destructible<ChildrenIterator<true>>::value, "");
	
	struct PathIterator: public BaseIterator {
		friend Tree;
	protected:
		PathIterator(Node* root): BaseIterator(root) {}
	public:
		PathIterator(const PathIterator& ii): BaseIterator(ii.current) {}
		PathIterator(PathIterator&& ii): BaseIterator(ii.current) {}
		virtual ~PathIterator() {}
		PathIterator& next() {
			if (this->current != nullptr) {
				this->current = this->current->parent;
			}
			return *this;
		}
		PathIterator operator++() {
			return next();
		}
		PathIterator operator++(int) {
			return next();
		}
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
	ChildrenIterator<false> begin_children(const BaseIterator& it) const {
		return ChildrenIterator<false>(it.current);
	}
	ChildrenIterator<false> end_children() const {
		return ChildrenIterator<false>();
	}
	ChildrenIterator<true> rbegin_children(const BaseIterator& it) const {
		return ChildrenIterator<true>(it.current);
	}
	ChildrenIterator<true> rend_children() const {
		return ChildrenIterator<true>();
	}
	PathIterator begin_path(const BaseIterator& it) const {
		return PathIterator(it.current);
	}
	PathIterator end_path() const {
		return PathIterator(nullptr);
	}
	
	BaseIterator setRoot(const T& data) {
		if (root == nullptr) root = new Node(data, nullptr);
		else root->data = data;
		return BaseIterator(root);
	}
	void clear() {
		delete root;
		root = nullptr;
	}
	BaseIterator insert(const T& data) {
		if (root == nullptr) setRoot(T());
		return insert(BaseIterator(root), data);
	}
	BaseIterator insert(BaseIterator position, const T& data) {
		Node n(data, position.current);
		std::size_t id = position.current->children.size();
		if (id > 0) n.previousSibling = &(position.current->children.back());
		position.current->children.push_back(n);
		if (id > 0) n.previousSibling->nextSibling = &(position.current->children.back());
		return BaseIterator(&(position.current->children.back()));
	}
	BaseIterator append(Tree&& tree) {
		if (root == nullptr) std::swap(root, tree.root);
		return append(BaseIterator(root), std::move(tree));
	}
	BaseIterator append(BaseIterator position, Tree&& data) {
		Node* r = data.root;
		data.root = nullptr;
		r->parent = position.current;
		std::size_t id = position.current->children.size();
		if (id > 0) r->previousSibling = &(position.current->children.back());
		position.current->children.push_back(*r);
		if (id > 0) r->previousSibling->nextSibling = &(position.current->children.back());
		return BaseIterator(&(position.current->children.back()));
	}
	
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
	void eraseChildren(const BaseIterator& position) {
		position.current->children.clear();
	}
	

	template<typename TT>
	friend std::ostream& operator<<(std::ostream& os, const Tree<TT>& tree) {
		for (auto it = tree.begin_preorder(); it != tree.end_preorder(); it++) {
			os << std::string(it.depth(), '\t') << *it << std::endl;
		}
		return os;
	}
	
	void dump(std::ostream& os, const Node& cur) const {
		os << cur.data << " next " << cur.nextSibling << " prev " << cur.previousSibling << " depth " << cur.depth << std::endl;
		for (const auto& c: cur.children) dump(os, c);
	}
	
	void dump(std::ostream& os) const {
		if (root == nullptr) return;
		dump(os, *root);
	}
};



}