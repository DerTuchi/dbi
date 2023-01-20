#include <iostream>
#include <vector>
using namespace std;

const int order = 4;


// BP node
class Node {
   public:
    int *key, size;
    bool leaf;
    Node **ptr;

    Node(){
        key = new int[order];
        ptr = new Node *[order+1];
        for (int i = 0; i < order+1;i++){
            ptr[i] = nullptr;
        }
    };


    void printNodeEntry(){
        cout<<"Keys:\t\t| ";
        for (int i=0; i < size; i++){
            cout<< key[i] <<" | ";
        }
        cout<<"\n\n";
    }
};

//BP tree
class BPTree {
   public:
    Node* root;

    BPTree(){
        root = nullptr;
    }

    void insert(int value){
        //  erstmaliges erstellen von Root;
        if (root == nullptr){
            root = new Node;
            root->key[0] = value;
            root->size = 1;
            root->leaf = true;
        }
        //  Falls root schon existiert wird nach dem ersten leaf geschaut indem die Value eingefügt werden soll
        else{
            Node *cursor = root, *parent;
            while (!(cursor->leaf)){
                parent = cursor;
                for (int i = 0; i < cursor->size; i++){
                    if (value < cursor->key[i]){
                        cursor = cursor->ptr[i];
                        break;
                    }
                    if (i == cursor->size-1){
                        cursor = cursor->ptr[i+1];
                        break;
                    }
                }
            }
            /*  Wenn noch Platz im Leaf ist und ein Wert größer im Leaf ist, wird für Value platzt gemacht im Node, indem
             *  die anderen aufrücken und dann an der freien stelle die Value eingefügt wird.*/
            if (cursor->size < order){
                int i = 0;
                while(i < cursor->size && cursor->key[i] < value){i++;}
                for (int j = cursor->size; j > i; j--){
                    cursor->key[j] = cursor->key[j-1];
                }
                cursor->key[i] = value;
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size -1];
                cursor->ptr[cursor->size -1] = nullptr;
            }
            /*  Falls das Leaf voll ist, wird trotzdem an der richtigen stelle das die value eingefügt und danach dieser
             *  Knoten geteilt. Diese beiden Leafs kriegen jetzt einen Elternknoten.
             *  Falls der geteilte Blattknoten gleichzeitig auch der Wurzelknoten war, wird ein neuer Wurzelknoten
             *  erstellt. Dieser hat dann die getrennten Blattknoten als Kinder.
             *  Falls schon ein Elternknoten vorhanden ist, wird die insertIntoNode funktion aufgerufen.
             * */
            else{
                Node *newLeaf = new Node;
                int tempNode[order+1];
                for (int i = 0; i < order; i++){
                    tempNode[i] = cursor->key[i];
                }
                int i = 0, j;
                while (i < order && tempNode[i] < value){
                    i++;
                }
                for(j = order+1; j > i; j--){
                    tempNode[j] = tempNode[j-1];
                }
                tempNode[i] = value;
                newLeaf->leaf = true;
                cursor->size = (order+1)/2;
                newLeaf->size = order+1-(order+1)/2;
                cursor->ptr[cursor->size] = newLeaf;
                newLeaf->ptr[newLeaf->size] = cursor->ptr[order];
                cursor->ptr[order] = nullptr;
                for (i = 0; i < cursor->size; i++){
                    cursor->key[i] = tempNode[i];
                }
                for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++){
                    newLeaf->key[i] = tempNode[j];
                }
                if(cursor == root){
                    Node *newRoot = new Node;
                    newRoot->key[0] = newLeaf->key[0];
                    newRoot->ptr[0] = cursor;
                    newRoot->ptr[1] = newLeaf;
                    newRoot->leaf = false;
                    newRoot->size = 1;
                    root = newRoot;
                }
                else{
                    insertIntoNode(newLeaf->key[0], parent, newLeaf);
                }
            }
        }
    }

    void insertIntoNode(int value, Node *cursor, Node *child){
        /*  Das Ziel der Funktion ist es in den Parent (hier cursor) das mittlere Element von vorher (hier value)
         *  zu integrieren und das Kind (hier child) mit dem Parent zu verbinden.
         * */
        /*  Wenn der Elternknoten noch Platz hat, wird an der passenden stelle die value eingefügt. Das selbe gilt für
         *  die Kinderpointer.
         * */
        if (cursor->size < order) {
            int i = 0;
            while (value > cursor->key[i] && i < cursor->size)
                i++;
            for (int j = cursor->size; j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
            }
            for (int j = cursor->size + 1; j > i + 1; j--) {
                cursor->ptr[j] = cursor->ptr[j - 1];
            }
            cursor->key[i] = value;
            cursor->size++;
            cursor->ptr[i + 1] = child;
        }
        /*  Ähnlich zur insert funktion wird andernfalls ein Temporäres Node element erstellt, welches den value mit dem child
         *  an der richtigen stelle einfügt und danach getrennt wird. Sonst verhält sich der Alg. gleich.
         *  Falls ein Parent element übergeben wurde, welches nicht Root ist, wird diese Funktion rekursiv aufgerufen,
         *  um dieses Parent richtig zuzuweisen.
         *  Um jedoch das richtige Parent Element zu finden wird die findParent funktion aufgerufen.
         * */
        else {
            Node *newInternal = new Node;
            int virtualKey[order + 1];
            Node *virtualPtr[order + 2];
            for (int i = 0; i < order; i++) {
                virtualKey[i] = cursor->key[i];
            }
            for (int i = 0; i < order + 1; i++) {
                virtualPtr[i] = cursor->ptr[i];
            }
            int i = 0, j;
            while (value > virtualKey[i] && i < order)
                i++;
            for (int j = order + 1; j > i; j--) {
                virtualKey[j] = virtualKey[j - 1];
            }
            virtualKey[i] = value;
            for (int j = order + 2; j > i + 1; j--) {
                virtualPtr[j] = virtualPtr[j - 1];
            }
            virtualPtr[i + 1] = child;
            newInternal->leaf = false;
            cursor->size = (order + 1) / 2;
            newInternal->size = order - (order + 1) / 2;
            for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
                newInternal->key[i] = virtualKey[j];
            }
            for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
                newInternal->ptr[i] = virtualPtr[j];
            }
            if (cursor == root) {
                Node *newRoot = new Node;
                newRoot->key[0] = cursor->key[cursor->size];
                newRoot->ptr[0] = cursor;
                newRoot->ptr[1] = newInternal;
                newRoot->leaf = false;
                newRoot->size = 1;
                root = newRoot;
            } else {
                insertIntoNode(cursor->key[cursor->size], findParent(root, cursor), newInternal);
            }
        }
    }

    Node* findParent(Node *cursor, Node *child){
        Node *parent;
        if (cursor->leaf || (cursor->ptr[0])->leaf) {
            return nullptr;
        }
        for (int i = 0; i < cursor->size + 1; i++) {
            if (cursor->ptr[i] == child) {
                parent = cursor;
                return parent;
            } else {
                parent = findParent(cursor->ptr[i], child);
                if (parent != nullptr)
                    return parent;
            }
        }
        return parent;
    }

    void printTree(Node *node, int depth=0){
        if(root== nullptr){
            cout<<"Tree is Empty\n";
        }
        if (node != nullptr){
            cout<<"| ";
            for (int i=0; i < node->size; i++){
                cout<< node->key[i] << " | ";
            }
            cout<<"\n\n";
            if (!(node->leaf)){
                for (int i = 0; i < node->size + 1; i++){
                    printTree(node->ptr[i], depth+1);
                }
            }
        }
    }

    void searchTree(int value){
        if (root == nullptr){
            cout<<"Tree is empty";
        }
        else{
            Node *cursor = root;
            while (!cursor->leaf){
                for (int i = 0; i < cursor->size; i++){
                    if (value < cursor->key[i]){
                        cursor = cursor->ptr[i];
                        break;
                    }
                    if (i == cursor->size - 1){
                        cursor = cursor->ptr[i + 1];
                        break;
                    }
                }
            }
            for (int i = 0; i < cursor->size; i ++){
                if (cursor->key[i] == value){
                    cout<<"Key found!\n";
                    return;
                }
            }
            cout<<"Key not found!\n";
        }
    }

    //... contains(...);
    //... delete(....);
};

// Insert Operation
//... BPTree::insert_key(...){}

// Search operation
//... BPTree::search(...){}

// Does the tree contain a certain node
//... BPTree::contains(...){}

// Delete a node
//... BPTree::search(...){}

// Print the tree
//... printTree(tree) {}

int main(void) {
    BPTree tree;
    tree.insert(0);
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);
    tree.insert(5);
    tree.insert(6);
    tree.printTree(tree.root);
    tree.searchTree(5);
    return 0;
}