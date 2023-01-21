#include <iostream>
#include <vector>
using namespace std;

int order = 6;


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


    void printNode(){
        cout<<"| ";
        for (int i=0; i < size; i++){
            cout<< key[i] <<" | ";
        }
        cout<<"\t\t";
    }
};

//BP tree
class BPTree {
   public:
    Node* root;

    BPTree(){
        root = nullptr;
    }

    void insertKey(int key){
        //  erstmaliges erstellen von Root;
        if (root == nullptr){
            root = new Node;
            root->key[0] = key;
            root->size = 1;
            root->leaf = true;
        }
        //  Falls root schon existiert wird nach dem ersten leaf geschaut indem die Value eingefügt werden soll
        else{
            Node *cursor = root, *parent;
            while (!(cursor->leaf)){
                parent = cursor;
                for (int i = 0; i < cursor->size; i++){
                    if (key < cursor->key[i]){
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
                while(i < cursor->size && cursor->key[i] < key){i++;}
                for (int j = cursor->size; j > i; j--){
                    cursor->key[j] = cursor->key[j-1];
                }
                cursor->key[i] = key;
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size -1];
                cursor->ptr[cursor->size -1] = nullptr;
            }
            /*  Falls das Leaf voll ist, wird trotzdem an der richtigen stelle das die key eingefügt und danach dieser
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
                while (i < order && tempNode[i] < key){
                    i++;
                }
                for(j = order+1; j > i; j--){
                    tempNode[j] = tempNode[j-1];
                }
                tempNode[i] = key;
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

    void insertIntoNode(int key, Node *cursor, Node *child){
        /*  Das Ziel der Funktion ist es in den Parent (hier cursor) das mittlere Element von vorher (hier key)
         *  zu integrieren und das Kind (hier child) mit dem Parent zu verbinden.
         * */
        /*  Wenn der Elternknoten noch Platz hat, wird an der passenden stelle die key eingefügt. Das selbe gilt für
         *  die Kinderpointer.
         * */
        if (cursor->size < order) {
            int i = 0;
            while (key > cursor->key[i] && i < cursor->size)
                i++;
            for (int j = cursor->size; j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
            }
            for (int j = cursor->size + 1; j > i + 1; j--) {
                cursor->ptr[j] = cursor->ptr[j - 1];
            }
            cursor->key[i] = key;
            cursor->size++;
            cursor->ptr[i + 1] = child;
        }
        /*  Ähnlich zur insertKey funktion wird andernfalls ein Temporäres Node element erstellt, welches den key mit dem child
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
            while (key > virtualKey[i] && i < order)
                i++;
            for (int j = order + 1; j > i; j--) {
                virtualKey[j] = virtualKey[j - 1];
            }
            virtualKey[i] = key;
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

    void nodesPerDepth(Node * node, int depthCurrent, int depth){
            if(depthCurrent != depth) {
                for (int i = 0; i < node->size+1;i++){
                    nodesPerDepth(node->ptr[i], depthCurrent + 1, depth);
                }
            } else{
                node->printNode();
                return;
            }

    }

    void printTree(){
        if(root== nullptr){
            cout<<"Tree is Empty\n";
        }
        int maxDepth = 0, currentDepth = 0;
        Node *cursor = root;
        while(!cursor->leaf){
            cursor = cursor->ptr[0];
            maxDepth ++;
        }
        while(currentDepth <= maxDepth){
            nodesPerDepth(root, 0, currentDepth);
                cout<<"\n\n";
                currentDepth ++;
        }
        cout<<"--------------------------------------------------------------------------------------\n";
    }

    Node *searchTree(int key){
        if (root == nullptr){
            cout<<"Tree is empty";
        }
        else{
            Node *cursor = root;
            while (!cursor->leaf){
                for (int i = 0; i < cursor->size; i++){
                    if (key < cursor->key[i]){
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
                if (cursor->key[i] == key){
                    //cout<<"Key found!\n";
                    return cursor;
                }
            }
            //cout<<"Key not found!\n";
            return nullptr;
        }
    }

    void deleteKey(int key){
        Node *cursor = searchTree(key);
        Node *parent;
        int leftSibling = 0, rightSibling = 0;
        if (cursor == nullptr){
            cout<<key<<" not in Tree";
            return;
        }
        cursor = root;
        bool condition = true;
        while (condition){
            parent = cursor;
            for (int i = 0; i < cursor->size; i++){
                if (key < cursor->key[i]){
                    cursor = cursor->ptr[i];
                    leftSibling = i-1;
                    rightSibling = i+1;
                    break;
                }
                if (i == cursor->size-1){
                    cursor = cursor->ptr[i + 1];
                    leftSibling = i;
                    rightSibling = i +2;
                    break;
                }
            }
            if(cursor != nullptr){
                for (int i = 0; i < cursor->size; i++){
                    if (cursor->key[i] == key){
                        condition = false;
                    }
                }
            }
        }
        if(cursor->leaf){
            // Wenn die Min-Größe, der Nodes eingehalten wird und der Key nur im Leaf vorhanden ist, kann man den einfach entfernen
            int i=0;
            while(true){
                if(cursor->key[i] == key){
                    break;
                }
                i++;
            }
            for (int j = i; j < cursor->size; j++){
                cursor->key[j] = cursor->key[j+1];
            }
            if(cursor->size < order/2){
                cursor->size --;
            }
            // Wird die Min-Größe NICHT eingehalten muss nimmt man ein element vom direkten Nachbarn.
            else{
                deleteKeyHelp(parent, cursor, rightSibling, leftSibling);
            }
        }
    }

    void deleteKeyHelp(Node* parent, Node* child, int rightSibling, int leftSibling){
        Node * cursor = child;
        if (leftSibling == -1){
            cursor->key[cursor->size] = parent->ptr[rightSibling]->key[0];
            for (int j = 0; j < parent->size; j ++){
                if(parent->key[j] == parent->ptr[rightSibling]->key[0]){
                    parent->key[j] = parent->ptr[rightSibling]->key[1];
                }
            }
            for (int j = 0; j < parent->ptr[rightSibling]->size; j ++) {
                parent->ptr[rightSibling]->key[j] = parent->ptr[rightSibling]->key[j + 1];
            }
            parent->ptr[rightSibling]->size --;
            if(parent->ptr[rightSibling]->size <= order/2 && parent->size > rightSibling){
                parent->ptr[rightSibling]->size ++;
                deleteKeyHelp(parent, cursor, rightSibling + 1, -1);
            }
            else{
                deleteKeyHelp(parent, cursor, rightSibling + 1, rightSibling);
            }

        }
        else if(rightSibling > parent->size){
            for (int i = 0; i < cursor->size; i ++){
                parent->ptr[leftSibling]->key[parent->ptr[leftSibling]->size+i] = cursor->key[i];
            }
            parent->ptr[parent->size+1] = nullptr;
            parent->size --;
        }
        //Wenn es Root ist nochmal anders handlen und auf andere Parents beziehen!!!

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
    int keys = 13;
    for (int i = 1; i <= keys; i ++){
        tree.insertKey(i);
    }
    tree.printTree();
    tree.deleteKey(2);
    tree.printTree();
    tree.insertKey(2);
    tree.printTree();
    tree.insertKey(3);
    tree.printTree();
    return 0;
}