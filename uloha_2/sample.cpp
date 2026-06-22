#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

#endif
using namespace std;

class CNode
{
public:

    int depth = 1;
    size_t treeSize = 1;
    char value;
    int newsInTree = 0;
    CNode* parent = nullptr;
    CNode* left = nullptr;
    CNode* right = nullptr;

    explicit CNode(char val) : value(val){}

    // true if value was changed
    bool countDepth()
    {
        auto tmp = 1 + max(((left == nullptr) ? 0 : left->depth), ((right == nullptr) ? 0 : right->depth));
        if(depth == tmp)
            return false;
        depth = tmp;
        return true;
    }

    bool countSize()
    {
        auto tmp = 1 + ((left == nullptr) ? 0 : left->treeSize) + ((right == nullptr) ? 0 : right->treeSize);
        if(treeSize == tmp)
            return false;
        treeSize = tmp;
        return true;
    }

    void countNews()
    {
        int tmp = (value == '\n') ? 1 : 0;
        tmp = tmp + ((left == nullptr) ? 0 : left->newsInTree) + ((right == nullptr) ? 0 : right->newsInTree);
        newsInTree = tmp;
    }


    int f_rotationValue()
    {
        rotationValue = (-1) * ((left == nullptr) ? 0 : left->depth) + ((right == nullptr) ? 0 : right->depth);
        return rotationValue;
    }
private:
    int rotationValue = 0;
};

// implementation is AVL tree with key being characters position
// count position via size of parent
struct TextEditorBackend {
public:

    explicit TextEditorBackend(const std::string& text)
    {
        for(size_t i = 0; i < text.size(); i++)
            mine_insert(i, text[i]);
    }

    ~TextEditorBackend()
    {
        queue<CNode*> toRemove;
        if(head != nullptr)
            toRemove.push(head);

        while(!toRemove.empty())
        {
            auto current = toRemove.front();
            toRemove.pop();
            if(current->left != nullptr)
            {
                toRemove.push(current->left);
            }
            if(current->right != nullptr)
            {
                toRemove.push(current->right);
            }
            delete current;
        }
    }

    size_t size() const
    {
        if(head == nullptr)
            return 0;
        return head->treeSize;
    }

    size_t lines() const
    {
        if(head == nullptr)
            return 1;
        return head->newsInTree + 1;
    }

    char at(size_t i) const
    {
        if(i >= size())
            throw out_of_range("out");
        return realFind(i)->value;

    }

    void edit(size_t i, char c)
    {
        if(i >= size())
            throw out_of_range("out");

        this->erase(i);
        this->insert(i,c);
    }

    void insert(size_t i, char c)
    {
        mine_insert(i,c);
    }

    // parent and which follower is it
    pair<CNode*, bool> find(size_t i) const
    {
        if (i > size())
            throw out_of_range("out");

        if(head == nullptr)
            return make_pair(nullptr, false);

        auto tmp = find_rec(head, i);
        if(tmp.second.first == size())
            return make_pair(nullptr, true);

        while (true)
        {
            if ((tmp.second.second) ? tmp.first->left == nullptr : tmp.first->right == nullptr)
                return make_pair(tmp.first, (tmp.second.second));
            else
            {
                auto temporary = find_rec((tmp.second.second) ? tmp.first->left : tmp.first->right, tmp.second.first);
                if (temporary.second.first == size() )
                {
                    return make_pair(tmp.first, tmp.second.second);
                }
                tmp = temporary;
            }
        }
    }

    // parent of found  + position in it + false (for right
    pair<CNode*, pair<size_t, bool>> find_rec(CNode* root, size_t position) const
    {
        if(root == nullptr)
            return make_pair(nullptr, make_pair(size(), true));
        uint sum = 0;

        // in left subtree
        if(root->left != nullptr)
        {
            if(position < root->left->treeSize)
                return make_pair(root, make_pair(position, true));
            else
                sum += root->left->treeSize;
        }

        // found
        if(sum == position)
            return make_pair(root, make_pair(size(), true));
        sum++;

        // in right subtree
        return make_pair(root, make_pair(position-sum, false));
    }

    void leftRotation(CNode* root)
    {

        root->right->parent = root->parent;
        root->parent = root->right;
        auto tmpD = root->right->left;
        auto tmpRoot = root;
        root = root->right;
        root->left = tmpRoot;
        root->left->right = tmpD;

        if(root->left->right != nullptr)
            root->left->right->parent = root->left;

        if(root->parent == nullptr)
            head = root;
        else
            (root->parent->left == root->left) ? root->parent->left = root : root->parent->right = root;

        root->left->countSize();
        root->left->countDepth();
        root->left->countNews();
        root->countSize();
        root->countDepth();
        root->countNews();
    }

    void rightRotation(CNode* root)
    {

        root->left->parent = root->parent;
        root->parent = root->left;
        auto tmpD = root->left->right;
        auto tmpRoot = root;
        root = root->left;
        root->right = tmpRoot;
        root->right->left = tmpD;

        if(root->right->left != nullptr)
            root->right->left->parent = root->right;

        if(root->parent == nullptr)
            head = root;
        else
            (root->parent->left == root->right) ? root->parent->left = root : root->parent->right = root;

        root->right->countSize();
        root->right->countDepth();
        root->right->countNews();
        root->countSize();
        root->countDepth();
        root->countNews();
    }

    void rotateTree(CNode* &rotationRoot)
    {
        if(rotationRoot->f_rotationValue() < 0) // rotation right
        {
            if(rotationRoot->left->f_rotationValue() > 0) // LR
                leftRotation(rotationRoot->left);

            rightRotation(rotationRoot);
        }
        else // rotation left
        {
            if(rotationRoot->right->f_rotationValue() < 0) // RL
                rightRotation(rotationRoot->right);

            leftRotation(rotationRoot);
        }
    }


    void propagateValueAndRotate(CNode* val)
    {
        auto parentTmp = val;

        while(parentTmp)
        {
            parentTmp->countSize();
            parentTmp->countDepth();
            parentTmp->countNews();

            if(parentTmp->f_rotationValue() <= -2 || parentTmp->f_rotationValue() >= 2)
                rotateTree(parentTmp);

            parentTmp = parentTmp->parent;
        }

    }

    CNode* realFind(size_t i) const
    {
        auto tmp = find(i);
        if(tmp.first == nullptr)
            return head;
        else if(tmp.second)
            return tmp.first->left;
        else
            return tmp.first->right;
    }


    CNode* mine_insert(size_t i, char c)
    {
        if(i < 0 || i > size())
            throw out_of_range("out");

        if(head == nullptr)
        {
            head = new CNode(c);
            head->countNews();
            return head;
        }


        CNode* val = new CNode(c);
        // points to parent element with indicator of direction
        auto seeked = find(i);

        CNode* temp;
        // parent is null - itself is fokin head
        if(seeked.first == nullptr)
            temp = head;
        else
        //  element itself, used when it is not null
            temp = seeked.second ? seeked.first->left : seeked.first->right;

        // neighbours of seeked are free so we insert directly
        if(temp == nullptr )
        {
            seeked.second ? seeked.first->left = val : seeked.first->right = val;
            val->parent = seeked.first;
        }
        // we go to the left and then straight to right corner
        else if(temp->left == nullptr)
        {
            temp->left = val;
            val->parent = temp;
        }
        else
        {
            CNode* tmp = temp->left;
            while(tmp->right != nullptr)
                tmp = tmp->right;

            val->parent = tmp;
            tmp->right = val;
        }

        // propagate tree size
        propagateValueAndRotate(val);

        return val;
    }

    void erase(size_t i, bool called = false)
    {
        if(i >= size())
            throw out_of_range("out");

        auto found = realFind(i);

        if(found->left == nullptr && found->right == nullptr) // no subtrees
        {
            auto par = found->parent;
            if(par == nullptr)
                head = nullptr;
            else
                (found->parent->left == found) ? found->parent->left = nullptr : found->parent->right = nullptr;

            propagateValueAndRotate(par);

            delete found;
        }
        else if(found->left == nullptr || found->right == nullptr) // only one subtree
        {
            auto heir = (found->left == nullptr) ? found->right : found->left;
            if(found->parent == nullptr)
                head = heir;
            else
                (found->parent->left == found) ? found->parent->left = heir : found->parent->right = heir;

            heir->parent = found->parent;

            propagateValueAndRotate(heir);
            delete found;
        }
        else // worst case scenario
        {
            auto tmp = found->left;
            while(tmp->right != nullptr)
                tmp = tmp->right;
            //remove tmp now
            auto ch = found->value;
            found->value = tmp->value;
            tmp->value = ch;
            // switch value with one that don't have right neighbour - erase that one (it will get done in first two if)
            erase(i-1, true);

        }
    }



    // find end of r - 1 row and add 1
    size_t line_start(size_t r) const
    {
        if(lines() <= r)
        {
            throw out_of_range("out");
        }

        if(r == 0)
            return 0;

        uint partial = r - 1;
        CNode* current = head;
        uint elemToLeft = 0;
        while(true)
        {
            uint leftCount = (current->left == nullptr) ? 0 : current->left->newsInTree;

            if(partial < leftCount) // going left
                current = current->left;
            else if(partial == leftCount && current->value == '\n') // we here
            //add elem in left subtree of current
            {
                elemToLeft += (current->left == nullptr) ? 0 : current->left->treeSize;
                return elemToLeft + 1;
            }
            else // going right
            {
                elemToLeft += (current->left == nullptr) ? 1 : (current->left->treeSize + 1);
                partial = partial - (current->newsInTree - current->right->newsInTree);
                current = current->right;
            }
        }
    }

    size_t line_length(size_t r) const
    {
        auto tmp = lines();
        if(tmp <= r)
            throw out_of_range("out");
        if(r == 0)
        {
            if(lines() == 1)
                return size();

            return line_start(1);
        }
        else if(r == tmp - 1)
        {
            return size() - line_start(r);
        }
        else
        {
            return line_start(r + 1) - line_start(r);
        }
    }

    size_t char_to_line(size_t i) const
    {
        if (i >= size())
            throw out_of_range("out");

        uint posInTree = i;
        CNode* current = head;
        uint newsLeft = 0;
        while(true)
        {
            uint leftCount = (current->left == nullptr) ? 0 : current->left->treeSize;

            if(leftCount > posInTree) // go left
            {
                current = current->left;
            }
            else if(leftCount == posInTree) // we here
            {
                newsLeft += (current->left == nullptr) ? 0 : current->left->newsInTree;
                return newsLeft;
            }
            else    // go right
            {
                newsLeft += (current->left == nullptr) ? 0 : (current->left->newsInTree);
                newsLeft += (current->value == '\n') ? 1 : 0;
                posInTree = posInTree - ((current->left == nullptr) ? 1 : (current->left->treeSize + 1));
                current = current->right;
            }
        }
    }

    void printTree() const
    {
        for(uint i = 0; i < size(); i++)
        {
            char tmp = at(i);
            if(tmp == '\n')
                cout << "& ";
            else
                cout << tmp << " ";
        }
        cout << endl;
    }

private:
    CNode* head = nullptr;
};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...) do { \
    std::array _arr = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < _arr.size(); _i++) \
      CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
  } while (0)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)
 
////////////////// End of dark magic ////////////////////////


std::string text(const TextEditorBackend& t) {
  std::string ret;
  for (size_t i = 0; i < t.size(); i++) ret.push_back(t.at(i));
  return ret;
}

void test1(int& ok, int& fail) {
	TextEditorBackend s("123\n456\n789");
  CHECK(s.size(), 11);
	CHECK(text(s), "123\n456\n789");
  CHECK(s.lines(), 3);
  CHECK_ALL(s.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2);
  CHECK_ALL(s.line_start, 0, 4, 8);
  CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK(t.size(), 12);
	CHECK(text(t), "123\n456\n789\n");
  CHECK(t.lines(), 4);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2,2);
  CHECK_ALL(t.line_start, 0, 4, 8, 12);
  CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int& ok, int& fail) {
	TextEditorBackend t("asdfasdfasdf");

  CHECK(t.size(), 12);
  CHECK(text(t), "asdfasdfasdf");
	CHECK(t.lines(), 1);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	CHECK(t.line_start(0), 0);
	CHECK(t.line_length(0), 12);
	
	t.insert(0, '\n');
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasdfasdf");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
	
  t.insert(4, '\n');
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasdfasdf");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);
	
  t.insert(t.size(), '\n');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdf\n");
	CHECK(t.lines(), 4);
  CHECK_ALL(t.line_start, 0, 1, 5, 15);
	
	t.edit(t.size() - 1, 'H');
  CHECK(t.size(), 15);


    CHECK(text(t), "\nasd\nfasdfasdfH");

  CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);

	t.erase(8);
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);

  t.erase(4);
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasfasdfH");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
}

void test_ex(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK_EX(t.at(12), std::out_of_range);

  CHECK_EX(t.insert(13, 'a'), std::out_of_range);
  CHECK_EX(t.edit(12, 'x'), std::out_of_range);
  CHECK_EX(t.erase(12), std::out_of_range);

  CHECK_EX(t.line_start(4), std::out_of_range);
  CHECK_EX(t.line_start(40), std::out_of_range);
  CHECK_EX(t.line_length(4), std::out_of_range);
  CHECK_EX(t.line_length(6), std::out_of_range);
  CHECK_EX(t.char_to_line(12), std::out_of_range);
  CHECK_EX(t.char_to_line(25), std::out_of_range);
}

void yeahBuddy()
{
    int ok = 0, fail = 0;
    if (!fail) test1(ok, fail);
    if (!fail) test2(ok, fail);
    if (!fail) test_ex(ok, fail);
    if (!fail) test3(ok, fail);

    if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
    else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

void myTest1()
{
    TextEditorBackend txt("abc");
//    txt.printTree();
    txt.insert(3,'d');
//    txt.printTree();
    txt.insert(0,'e');
//    txt.printTree();
    txt.insert(5,'f');
//    txt.printTree();
}

void myTest2()
{
    TextEditorBackend txt("abc");
    txt.erase(0);
    txt.erase(0);
    txt.erase(0);
    cout << "erase all\n";
    try {
        txt.erase(0);
    }
    catch (exception& e){}
    cout << "tree\n";
//    txt.printTree();
    txt.insert(0, '\n');
    txt.erase(0);
}

void myTest3()
{
    TextEditorBackend txt("");
    for(uint i = 0; i < 5000; i++) //500000 still somewhat manageable
    {
        txt.insert(0, 'l');
    }

    txt.insert(0, 'e');
//    txt.printTree();
    txt.insert(0, 'd');
//    txt.printTree();
    txt.insert(0, 'c');
//    txt.printTree();
    txt.insert(0, 'b');
//    txt.printTree();
    txt.insert(0, 'a');
//    txt.printTree();

    cout << txt.size() << endl;
//    txt.erase(1);
//    txt.erase(0);

    while(txt.size() > 0)
        txt.erase(0); // with 3rd iteration

    cout << "time test" << endl;
}

void myTest4()
{
    TextEditorBackend txt("abc");
    cout << "gehj" << endl;
    txt.erase(1);
}

void myTest0()
{
    TextEditorBackend txt("abc\n");
    cout << "lines here : " << txt.lines() << endl;
}

void myTest5()
{
    TextEditorBackend txt("");
    txt.insert(0, '\n');
    txt.insert(1, 'n');
    txt.edit(1, 'k');
    txt.edit(1, '\n');
    txt.erase(0);
    txt.insert(0, 'k');
}

void myTest6()
{
    TextEditorBackend txt("iqsfj\nmvt");
    txt.edit(4, '\n');
    cout << "tvojamama" << endl;
}

int main()
{
    TextEditorBackend txt("");
    assert(txt.lines() == 1);
    myTest0();
    yeahBuddy();
    myTest1();
    myTest2();
    myTest3();
    myTest4();
    myTest5();
    myTest6();
    cout << "tvojemama" << endl;
    return 0;
}



#endif


