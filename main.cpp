#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

using namespace std;

// node in red black tree
struct Node
{
    int value;

    // black = 0 ; red = 1;
    int color;

    Node* left;
    Node* right;
    Node* parent;
};

// red black tree
class RBTree
{
    private:
        Node* root;
        int insert_arr[200];
        int insert_idx;
        int delete_arr[200];
        int delete_idx;

    public:
        // constructor
        RBTree()
        {
            root = NULL;
            insert_idx = 0;
            delete_idx = 0;
        }

        // insertion driver
        void Insertion(string insertion_str)
        {
            stringstream ss;
            string temp="";
            int n;

            ss<<insertion_str;

            while(!ss.eof())
            {
                ss>>temp;
                if(stringstream(temp)>>n)
                {
                    insert_arr[insert_idx] = n;
                    insert_idx++;
                    InsertionHelper(n);
                }

                temp="";
            }
        }

        // insertion hepler
        void InsertionHelper(int n)
        {
            // create new node
            Node* new_node = CreateNewNode(n);
            Node* parent_node = NULL;

            // no node in red black tree
            if(root==NULL)
            {
                root = new_node;
            }

            // at least one node in red black tree
            else
            {
                // binary search tree way to find parent node
                parent_node = BSTInsert(n);

                // DEBUG
                if(parent_node==NULL)
                    cout<<"BUG : find no parent node !";

                // insert new node as left / right child node
                if(n>=parent_node->value)
                {
                    parent_node->right = new_node;
                    new_node->parent = parent_node;
                }
                else
                {
                    parent_node->left = new_node;
                    new_node->parent = parent_node;
                }

                // check if violate red black tree rule
                if(parent_node->color == 1)
                    FixInsertionViolation(new_node);
            }

            // make root node black (property of red black tree)
            root->color = 0;
        }

        // fix insertion violation
        void FixInsertionViolation(Node* new_node)
        {
            // violation node is root, convert it to black
            if(new_node==root)
                new_node->color = 0;

            // if parent node is black will not violate rule
            else if(new_node->parent->color==0)
                return;

            else
            {
                Node* uncle_node = GetUncleNode(new_node);
                Node* parent_node = new_node->parent;
                Node* grandparend_node = parent_node->parent;

                // case A : uncle node is also red
                if(uncle_node!=NULL && uncle_node->color==1)
                {
                        // convert both parent node and uncle node to black
                        // then convert grandparent node to red
                        // consider grandparent node as new insert node
                        parent_node->color = 0;
                        uncle_node->color = 0;
                        grandparend_node->color = 1;
                        FixInsertionViolation(grandparend_node);
                }

                // case B : uncle node is black or no uncle node
                else
                {
                    // consider four case

                    // left left case
                    if(grandparend_node->left == parent_node && parent_node->left == new_node)
                    {
                        RightRotation(grandparend_node);
                        SwapColor(parent_node,grandparend_node);
                    }

                    // left right case
                    else if(grandparend_node->left == parent_node && parent_node->right == new_node)
                    {
                        LeftRotation(parent_node);
                        RightRotation(grandparend_node);
                        SwapColor(new_node,grandparend_node);
                    }

                    // right right case
                    else if(grandparend_node->right == parent_node && parent_node->right == new_node)
                    {
                        LeftRotation(grandparend_node);
                        SwapColor(parent_node,grandparend_node);
                    }

                    // right left case
                    else
                    {
                        RightRotation(parent_node);
                        LeftRotation(grandparend_node);
                        SwapColor(new_node,grandparend_node);
                    }

                }
            }
        }

        // deletion driver
        void Deletion(string deletion_str)
        {
            stringstream ss;
            string temp="";
            int n;

            ss<<deletion_str;

            while(!ss.eof())
            {
                ss>>temp;
                if(stringstream(temp)>>n)
                {
                    delete_arr[delete_idx] = n;
                    delete_idx++;
                    DeletionHelper(n,root);
                }

                temp="";
            }
        }

        // deletion helper
        void DeletionHelper(int n, Node* r)
        {
            // find the node should be deleted
            Node* delete_node = BSTSearch(n,r);

            // if delete node has two child, find his inorder successsor
            if(delete_node->left!=NULL && delete_node->right!=NULL)
            {
                Node* successor_node = FindSuccessor(delete_node->right);
                delete_node->value = successor_node->value;
                DeletionHelper(successor_node->value,delete_node->right);
            }
            else
            {
                // in this part, delete node only has 0 or 1 child

                // find parent node and child node
                Node* parent_node = delete_node->parent;
                Node* child_node;
                if(delete_node->left!=NULL)
                    child_node = delete_node->left;
                else
                    child_node = delete_node->right;


                // delete node is red
                if(delete_node->color == 1)
                {
                    if(parent_node->left == delete_node)
                        parent_node->left = child_node;
                    else
                        parent_node->right = child_node;

                    if(child_node!=NULL)
                        child_node->parent = parent_node;
                }

                // delete node is black
                else
                {
                    // delete node is root
                    if(delete_node == root)
                    {
                        root = child_node;
                        if(child_node!=NULL)
                            child_node->parent = NULL;
                    }

                    // delete node is not root
                    else
                    {
                        // has one child
                        if(child_node!=NULL)
                        {
                            child_node->parent = parent_node;
                            if(parent_node->left == delete_node)
                                parent_node->left = child_node;
                            else
                                parent_node->right = child_node;

                            if(child_node->color == 1)
                                child_node->color = 0;
                            else
                                FixDeletionViolation(child_node);
                        }

                        // no child
                        else
                        {
                            FixDeletionViolation(delete_node);
                            if(delete_node->parent->left == delete_node)
                                delete_node->parent->left = NULL;
                            else
                                delete_node->parent->right = NULL;
                        }
                    }
                }

                delete delete_node;
            }
        }

        // fix deletion violation
        void FixDeletionViolation(Node* n_node)
        {
            Node* parent_node = n_node->parent;

            // condition 1
            if(n_node == root)
            {
                return;
            }

            // condition 2
            if(GetParentColor(n_node)==0 && GetSiblingColor(n_node)==1 && GetSiblingLeftColor(n_node)==0 && GetSiblingRightColor(n_node)==0)
            {
                if(parent_node->left == n_node)
                    LeftRotation(parent_node);
                else
                    RightRotation(parent_node);

                SwapColor(parent_node,parent_node->parent);
            }

            // condition 3
            if(GetParentColor(n_node)==0 && GetSiblingColor(n_node)==0 && GetSiblingLeftColor(n_node)==0 && GetSiblingRightColor(n_node)==0)
            {
                if(parent_node->left == n_node)
                    parent_node->right->color = 1;
                else
                    parent_node->left->color = 1;

                FixDeletionViolation(parent_node);
                return;
            }

            // condition 4
            if(GetParentColor(n_node)==1 && GetSiblingColor(n_node)==0 && GetSiblingLeftColor(n_node)==0 && GetSiblingRightColor(n_node)==0)
            {
                parent_node = 0;
                if(parent_node->left == n_node)
                    parent_node->right->color = 1;
                else
                    parent_node->right->color = 1;
                return;
            }

            // condition 5
            if(GetSiblingColor(n_node)==0)
            {
                if(parent_node->left ==n_node && GetSiblingLeftColor(n_node)==1 && GetSiblingRightColor(n_node)==0)
                {
                    Node* sibling_node = parent_node->right;
                    Node* sibling_left = sibling_node->left;

                    RightRotation(sibling_node);
                    SwapColor(sibling_node,sibling_left);
                }

                if(parent_node->right == n_node && GetSiblingLeftColor(n_node)==0 && GetSiblingRightColor(n_node)==1)
                {
                    Node* sibling_node = parent_node->left;
                    Node* sibling_right = sibling_node->right;

                    LeftRotation(sibling_node);
                    SwapColor(sibling_node,sibling_right);
                }
            }

            // condition 6
            if(GetSiblingColor(n_node)==0 && (GetSiblingLeftColor(n_node)==1 || GetSiblingRightColor(n_node)==1))
            {
                if(parent_node->left == n_node && GetSiblingRightColor(n_node)==1)
                {
                    LeftRotation(parent_node);
                    SwapColor(parent_node,parent_node->parent);
                    parent_node->parent->right->color = 0;
                }

                if(parent_node->right == n_node && GetSiblingLeftColor(n_node)==1)
                {
                    RightRotation(parent_node);
                    SwapColor(parent_node,parent_node->parent);
                    parent_node->parent->left->color = 0;
                }

                return;
            }
        }

        // right rotation
        void RightRotation(Node* temp_node)
        {
            Node* left_child = temp_node->left;

            temp_node->left = left_child->right;
            if(left_child->right!=NULL)
                left_child->right->parent = temp_node;

            left_child->parent = temp_node->parent;
            if(temp_node->parent!=NULL)
            {
                if(temp_node->parent->right == temp_node)
                    temp_node->parent->right = left_child;
                else
                    temp_node->parent->left = left_child;
            }

            left_child->right = temp_node;
            temp_node->parent = left_child;

            if(temp_node==root)
                root = left_child;
        }

        // left rotation
        void LeftRotation(Node* temp_node)
        {
            Node* right_child = temp_node->right;

            temp_node->right = right_child->left;
            if(right_child->left!=NULL)
                right_child->left->parent = temp_node;

            right_child->parent = temp_node->parent;
            if(temp_node->parent!=NULL)
            {
                if(temp_node->parent->right == temp_node)
                    temp_node->parent->right = right_child;
                else
                    temp_node->parent->left = right_child;
            }

            temp_node->parent = right_child;
            right_child->left = temp_node;

            if(temp_node==root)
                root = right_child;
        }

        // swap color of two node
        void SwapColor(Node* a_node, Node* b_node)
        {
            int c = a_node->color;
            a_node->color = b_node->color;
            b_node->color = c;
        }

        // return uncle node of new node
        Node* GetUncleNode(Node* new_node)
        {
            Node* parent_node = new_node->parent;
            Node* grandparent_node = parent_node->parent;

            if(grandparent_node==NULL)
                return NULL;

            if(grandparent_node->left == parent_node)
                return grandparent_node->right;
            else
                return grandparent_node->left;
        }

        // return parent node's color
        int GetParentColor(Node* n_node)
        {
            return n_node->parent->color;
        }

        // return sibling's color
        int GetSiblingColor(Node* n_node)
        {
            Node* parent_node = n_node->parent;
            if(parent_node->left == n_node)
                return parent_node->right->color;
            else
                return parent_node->left->color;
        }

        // return sibling's left child color
        int GetSiblingLeftColor(Node* n_node)
        {
            Node* parent_node = n_node->parent;
            Node* sibling_node;

            if(parent_node->left == n_node)
                sibling_node = parent_node->right;
            else
                sibling_node = parent_node->left;

            if(sibling_node->left == NULL)
                return 0;
            else
                return sibling_node->left->color;
        }

        // return sibling's right child color
        int GetSiblingRightColor(Node* n_node)
        {
            Node* parent_node = n_node->parent;
            Node* sibling_node;

            if(parent_node->left == n_node)
                sibling_node = parent_node->right;
            else
                sibling_node = parent_node->left;

            if(sibling_node->right == NULL)
                return 0;
            else
                return sibling_node->right->color;
        }

        // binary search way to insert new node
        Node* BSTInsert(int n)
        {
            Node* current = root;
            Node* previous;
            while(current!=NULL)
            {
                previous = current;

                if(n>=current->value)
                    current = current->right;
                else
                    current = current->left;
            }

            return previous;
        }

        // binary search way to find the node
        Node* BSTSearch(int n, Node* r)
        {
            Node* current = r;
            while(current!=NULL)
            {
                if(current->value == n)
                    return current;
                else
                {
                    if(n>=current->value)
                        current = current->right;
                    else
                        current = current->left;
                }
            }
        }

        // find the inorder successor
        Node* FindSuccessor(Node* r)
        {
            Node* current = r;
            while(current->left!=NULL)
                current = current->left;
            return current;
        }

        // create new node
        Node* CreateNewNode(int n)
        {
            Node* new_node = new Node;
            new_node->color = 1;
            new_node->value = n;
            new_node->left = NULL;
            new_node->right = NULL;
            new_node->parent = NULL;

            return new_node;
        }

        // print the format outout
        void PrintOutput(int mission)
        {
            // output of insertion
            if(mission==1)
            {
                cout<<"Insert:";
                cout<<" "<<insert_arr[0];
                for(int i=1;i<insert_idx;i++)
                {
                    cout<<",";
                    cout<<" "<<insert_arr[i];
                }
                cout<<endl;
                insert_idx=0;
            }

            // output of deletion
            else
            {
                cout<<"Delete:";
                cout<<" "<<delete_arr[0];
                for(int i=1;i<delete_idx;i++)
                {
                    cout<<",";
                    cout<<" "<<delete_arr[i];
                }
                cout<<endl;
                delete_idx=0;
            }

            // print inorder traversal of red black tree
            PrintInorder(root);
        }

        // print inorder traversal of red black tree
        void PrintInorder(Node* root)
        {
            if(root!=NULL)
            {
                PrintInorder(root->left);

                cout<<"key: "<<root->value;
                cout<<" ";

                if(root->parent==NULL)
                    cout<<"parent: "<<" ";
                else
                    cout<<"parent: "<<root->parent->value;
                cout<<" ";

                if(root->color==0)
                    cout<<"color: "<<"black";
                else
                    cout<<"color: "<<"red";
                cout<<endl;

                PrintInorder(root->right);
            }
        }
};

// convert a string into integer
int String2Int(string str)
{
    stringstream ss;
    int temp;
    ss<<str;
    ss>>temp;
    return temp;
}

// driver
int main()
{
    // read from file
    ifstream input("input.txt");

    // user input : number of missions
    string temp;
    int num_mission;
    getline(input,temp);
    num_mission = String2Int(temp);

    // build a red black tree object
    RBTree t;

    int mission;
    for(int i=0;i<num_mission;i++)
    {
        // user input : the content of mission
        getline(input,temp);
        mission = String2Int(temp);

        // classify the mission : insert or delete
        if(mission==1)
        {
            // insertion mission
            getline(input,temp);
            t.Insertion(temp);
            t.PrintOutput(mission);
        }
        else
        {
            // deletion mission
            getline(input,temp);
            t.Deletion(temp);
            t.PrintOutput(mission);
        }
    }

    return 0;
}
