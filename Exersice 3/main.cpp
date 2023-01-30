#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>

using namespace std;

int order = 2;

// BP node
class Node {
public:
    int *key, size;
    bool leaf;
    Node **ptr;
    mutex node_mutex;

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
    int test;
    BPTree(){
        root = nullptr;
        test = 0;
    }

    void insertKey(int key){
        if(key == 8){
            //3mal
            test ++;
        }
        //  erstmaliges erstellen von Root;
        if (root == nullptr){
            root = new Node;

            unique_lock<mutex> lock(root->node_mutex);

            root->key[0] = key;
            root->size = 1;
            root->leaf = true;
            return;
        }
        //  Falls root schon existiert wird nach dem ersten leaf geschaut indem die Value eingefügt werden soll
        else{
            Node *cursor = root, *parent;
            int leftChild;
            while (!(cursor->leaf)){
                parent = cursor;
                for (int i = 0; i < cursor->size; i++){
                    if (key < cursor->key[i]){
                        cursor = cursor->ptr[i];
                        leftChild = i-1;
                        break;
                    }
                    if (i == cursor->size-1){
                        cursor = cursor->ptr[i+1];
                        leftChild = i;
                        break;
                    }
                }
            }
            /*  Wenn noch Platz im Leaf ist und ein Wert größer im Leaf ist, wird für Value platzt gemacht im Node, indem
             *  die anderen aufrücken und dann an der freien stelle die Value eingefügt wird.*/
            if(cursor->key[0] == key && cursor->size == order){
                if(leftChild != -1){
                    //TODO: SEARCH ALTERNATIVE FUNCTION EINFÜGEN
                    cursor = searchAlternativeNode(key);
                    if(cursor == nullptr){
                        cursor = parent->ptr[leftChild];
                    }
                }
                else{
                    cursor = root;
                    while (!(cursor->leaf)){
                        parent = cursor;
                        for (int i = 0; i < cursor->size; i++){
                            if (key-1 < cursor->key[i]){
                                cursor = cursor->ptr[i];
                                leftChild = i-1;
                                break;
                            }
                            if (i == cursor->size-1){
                                cursor = cursor->ptr[i+1];
                                leftChild = i;
                                break;
                            }
                        }
                    }
                }
            }
            if (cursor->size < order){

                unique_lock<mutex> l1(cursor->node_mutex);

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

                unique_lock<mutex> l1(cursor->node_mutex);
                unique_lock<mutex> l2(newLeaf->node_mutex);

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

                    unique_lock<mutex> l3(newRoot->node_mutex);

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

        int newKey = 0;
        if (cursor->size < order) {
            int i = 0;
            while (key > cursor->key[i] && i < cursor->size){
                i++;
            }

            unique_lock<mutex> l1(cursor->node_mutex);

            for (int j = cursor->size; j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
            }
            for (int j = cursor->size+1; j > i + 1; j--) {
                cursor->ptr[j] = cursor->ptr[j-1];
            }

            for(int j = 0; j < cursor->size; j ++){
                if(key == cursor->key[j] && cursor->ptr[j+1]->key[cursor->ptr[j+1]->size- 1] < child->key[child->size-1]){
                    cursor->ptr[j+2] = child;
                    cursor->key[i] = key;
                    cursor->size++;
                    return;
                }
            }

            cursor->key[i] = key;
            cursor->size++;
            cursor->ptr[i+1] = child;
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
            int virtualKeyCopy[order+1];
            Node *virtualPtr[order + 2];
            Node *virtualPtrCopy[order + 2];
            for (int i = 0; i < order; i++) {
                virtualKey[i] = cursor->key[i];
                virtualKeyCopy[i] = cursor->key[i];
            }
            for (int i = 0; i < order + 1; i++) {
                virtualPtr[i] = cursor->ptr[i];
                virtualPtrCopy[i] = cursor->ptr[i];
            }
            int i = 0, j;
            while (key > virtualKey[i] && i < order)
                i++;
            for (int j = order + 1; j > i; j--) {
                virtualKey[j] = virtualKey[j - 1];
                virtualKeyCopy[j] = virtualKeyCopy[j - 1];
            }
            virtualKey[i] = key;
            virtualKeyCopy[i] = key;

            //WICHTIGE STELLE MIT DEM NEWKEY
            newKey = virtualKeyCopy[(order + 1)/2];

            if(virtualPtrCopy[order]->key[virtualPtrCopy[order]->size-1] < child->key[child->size-1]) virtualPtr[order+1] = child;
            else{
                for (int j = order + 2; j > i + 1; j--) {
                    virtualPtr[j] = virtualPtr[j - 1];
                }
                virtualPtr[i + 1] = child;
            }
            unique_lock<mutex> l1(cursor->node_mutex);

            cursor->size = (order + 1) / 2;

            unique_lock<mutex> l2(newInternal->node_mutex);

            newInternal->leaf = false;
            newInternal->size = order - (order + 1) / 2;

            if(cursor->key[0] != virtualKey[0]){
                for(i = 0; i < cursor->size; i++){
                    cursor->key[i] = virtualKey[i];
                }
                for(i = 0; i < cursor->size + 1; i++){
                    cursor->ptr[i] = virtualPtr[i];
                }
            }

            for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
                newInternal->key[i] = virtualKey[j];
            }
            for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
                newInternal->ptr[i] = virtualPtr[j];
            }
            if (cursor == root) {
                Node *newRoot = new Node;

                unique_lock<mutex> l3(newRoot->node_mutex);
                newRoot->key[0] = newKey;
                newRoot->ptr[0] = cursor;
                newRoot->ptr[1] = newInternal;
                newRoot->leaf = false;
                newRoot->size = 1;
                root = newRoot;
            } else {
                insertIntoNode(newKey, findParent(root, cursor), newInternal);
            }
        }
    }

    Node* searchAlternativeNode(int key){
        Node *cursor = root, *parent;
        int rightChild, leftChild;
        while (!(cursor->leaf)){
            parent = cursor;
            for (int i = 0; i < cursor->size; i++){
                if (key - 1 < cursor->key[i]){
                    cursor = cursor->ptr[i];
                    rightChild = i+1;
                    break;
                }
                if (i == cursor->size-1){
                    cursor = cursor->ptr[i+1];
                    rightChild = i+2;
                    break;
                }
            }
        }
        if(rightChild <= parent->size && parent->ptr[rightChild]->size < order){
            return parent->ptr[rightChild];
        }
        return nullptr;
    }


    Node* findParent(Node *cursor, Node *child){
        Node *parent;
        if (cursor->leaf || (cursor->ptr[0])->leaf) {
            return NULL;
        }
        for (int i = 0; i < cursor->size + 1; i++) {
            if (cursor->ptr[i] == child) {
                parent = cursor;
                return parent;
            } else {
                parent = findParent(cursor->ptr[i], child);
                if (parent != NULL)
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
                    return cursor;
                }
            }
            return nullptr;
        }
    }

    void deleteKey(int key){
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node *cursor = searchTree(key);
        Node *parent = nullptr;
        bool condition = true;
        int leftSibling = 0, rightSibling = 0;
        if (cursor == nullptr){
            cout<<key<<" not in Tree";
            return;
        }
        cursor = root;
        for (int i = 0; i < cursor->size; i++){
            if (cursor->key[i] == key){
                parent = cursor;
                condition = false;
            }
        }
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
            for (int i = 0; i < cursor->size; i++){
                if (cursor->key[i] == key && cursor->leaf){
                    condition = false;
                }
            }
        }
        // Wenn die Min-Größe, der Nodes eingehalten wird und der Key nur im Leaf vorhanden ist, kann man den einfach entfernen
        int i=0;
        while(true){
            if(cursor->key[i] == key){
                break;
            }
            i++;
        }
        for (int j = i; j < cursor->size -1; j++){
            cursor->key[j] = cursor->key[j+1];
        }
        cursor->size --;
        // Wird die Min-Größe NICHT eingehalten muss nimmt man ein element vom direkten Nachbarn.
        if (cursor->size < threshhold){
            if(rightSibling <= cursor->size + 1){
                if(!getKeyFromSibling(parent, cursor, rightSibling, false)){
                    mergeNodes(key, parent,cursor, rightSibling);
                }
            }
            else if(leftSibling >= 0){
                if(!getKeyFromSibling(parent, cursor, leftSibling, true)){
                    mergeNodes(parent->ptr[leftSibling]->key[parent->ptr[leftSibling]->size-1], parent,cursor, leftSibling);
                }

            }
        }
    }

    bool getKeyFromSibling(Node *parent, Node* cursor, int sibling, bool leftSibling){
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node *child = parent->ptr[sibling];
        // Rechtes Node ein Element nehmen
        if(!leftSibling &&child->size > threshhold){
            // Falls der Parent einen anderen Wert hat, muss dieser beim Klauen des nachbarn berücksichtigt werden
            if(parent->key[sibling-1] != child->key[0]){
                cursor->key[cursor->size] = parent->key[sibling-1];
                parent->key[sibling-1] = child->key[0];
                for (int i = 0; i < child->size; i ++){
                    child->key[i] = child->key[i+1];
                }
                cursor->ptr[cursor->size + 1] = child->ptr[0];
                for(int i = 0; i < child->size + 1; i ++){
                    child->ptr[i] = child->ptr[i+1];
                }
                child->size--;
                cursor->size ++;
            }
            // Andernfalls kann man die Werte einfach Klauen und Löschen
            else{
                cursor->key[cursor->size] = child->key[0];
                cursor->size ++;
                for (int i = 0; i < child->size; i ++){
                    child->key[i] = child->key[i+1];
                }
                child->size--;
                parent->key[sibling - 2] = cursor->key[0];
                parent->key[sibling - 1] = child->key[0];
            }
            return true;
        }
        // Linkes Node ein element nehmen
        else if(leftSibling && child->size > threshhold){
            // Falls der Parent einen anderen Wert hat, muss dieser beim Klauen des nachbarn berücksichtigt werden
            if(parent->key[sibling] != cursor->key[0]){
                for(int i = 0; i < cursor->size; i ++){
                    cursor->key[i] = cursor->key[i+1];
                }
                cursor->key[0] = parent->key[sibling];
                parent->key[sibling] = child->key[child->size];
                for(int i = 0; i < cursor->size + 1; i ++){
                    cursor->ptr[i] = cursor->ptr[i+1];
                }
                cursor->ptr[0] = child->ptr[child->size + 1];
                child->size --;
                cursor->size ++;
            }
            // Andernfalls kann man die Werte einfach Klauen und Löschen
            else{
                for(int i = cursor->size; i > 0; i --){
                    cursor->key[i] = cursor->key[i-1];
                }
                cursor->key[0] = child->key[child->size-1];
                cursor->size ++;
                child->size --;
                parent->key[sibling] = cursor->key[0];
            }
            return true;
        }
        return false;
    }

    void mergeNodes(int key, Node *parent, Node *cursor, int mergePartner){
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node * child = parent->ptr[mergePartner];
        //child ist rechts
        if(key < child->key[0]){
            //Schauen ob der Parent beim Merge einen anderen Wert hat und ggf mit einbeziehen
            if(child->key[0]!=parent->key[mergePartner - 1]){
                //Wert in Child einfügen
                for(int i = child->size; i > 0; i --){
                    child->key[i] = child->key[i-1];
                }
                child->key[0] = parent->key[mergePartner - 1];
                child->size++;
            }

            //Wert aus Parent entfernen
            int parentKeyLocation = 0;
            for(parentKeyLocation; parentKeyLocation < parent->size; parentKeyLocation++){
                if(parent->key[parentKeyLocation] == parent->key[mergePartner -1]){
                    break;
                }
            }
            for(parentKeyLocation; parentKeyLocation < parent->size; parentKeyLocation ++){
                parent->key[parentKeyLocation] = parent->key[parentKeyLocation+1];
            }
            parent->size --;

            // Wenn es kein Leaf ist, sollen auch die Pointer mit gemerged werden
            if(!cursor->leaf){
               for(int i = 0; i < child->size + 1; i++){
                   cursor->ptr[cursor->size + 1 + i] = child->ptr[i];
               }
            }
            //Mergen der Keys
            for(int i = 0; i < child->size; i ++){
                cursor->key[cursor->size + i] = child->key[i];
            }
        }
        //child ist links
        else{
            //Schauen ob der Parent beim Merge einen anderen Wert hat und ggf mit einbeziehen
            if(cursor->key[0] != parent->key[mergePartner]){
                child->key[child->size] = parent->key[mergePartner];
                child->size++;
                // Wenn es kein Leaf ist, sollen auch die Pointer mit gemerged werden
                if(!cursor->leaf){
                    Node *tempPtr [cursor->size +1 + child->size];
                    for(int i = 0; i < child->size; i++){
                        tempPtr[i] = child->ptr[i];
                    }
                    for(int i = 0; i < cursor->size + 1; i++){
                        tempPtr[child->size + i] = cursor->ptr[i];
                    }
                    for(int i = 0; i < cursor->size + child->size + 2; i ++){
                        cursor->ptr[i] = tempPtr[i];
                    }
                }
            }
            else{
                // Wenn es kein Leaf ist, sollen auch die Pointer mit gemerged werden
                if(!cursor->leaf){
                    Node *tempPtr [cursor->size+1 + child->size+1];
                    for(int i = 0; i < child->size + 1; i++){
                        tempPtr[i] = child->ptr[i];
                    }
                    for(int i = 0; i < cursor->size + 1; i++){
                        tempPtr[child->size + 1 + i] = cursor->ptr[i];
                    }
                    for(int i = 0; i < cursor->size + child->size + 2; i ++){
                        cursor->ptr[i] = tempPtr[i];
                    }
                }
            }
            //Wert aus Parent entfernen
            int parentKeyLocation = 0;
            for(parentKeyLocation; parentKeyLocation < parent->size; parentKeyLocation++){
                if(parent->key[parentKeyLocation] == parent->key[mergePartner]){
                    break;
                }
            }
            for(int i = parentKeyLocation; i < parent->size; i ++){
                parent->key[i] = parent->key[i + 1];
            }
            for(int i = parentKeyLocation; i < parent->size + 1; i ++){
                parent->ptr[i] = parent->ptr[i+1];
            }
            parent->size --;

            //Mergen der beiden Nodes
            int tempValues [cursor->size + child->size];
            for (int i = 0; i < child->size; i ++){
                tempValues[i] = child->key[i];
            }
            for(int i = 0; i < cursor->size; i++){
                tempValues[child->size + i] = cursor->key[i];
            }

            for(int i = 0; i < cursor->size + child->size; i ++){
                cursor->key[i] = tempValues[i];
            }
        }
        cursor->size = cursor->size + child->size;

        //Löschen des childs
        delete[] child->key;
        delete[] child->ptr;
        delete child;

        //Wenn Parent Root ist und size = 0, wird cursor zum root
        if(parent == root && parent->size == 0){
            root = cursor;
            return;
        }
        //Rekursion, falls Parent den Threshhold nicht einhalten kann und kein root ist
        if (parent->size < threshhold && parent != root){
            Node *newParent = findParent(root, parent);
            int left, right;
            bool rightAsChild = true, leftAsChild = false;
            for(int i = 0; i < newParent->size + 1; i ++){
                left = i - 1;
                right = i + 1;
                if(i == newParent->size && newParent->ptr[i] == parent){
                    rightAsChild = false;
                    leftAsChild = true;
                    break;
                }
                if(newParent->ptr[i] == parent){ break;}
            }

            if(rightAsChild && !getKeyFromSibling(newParent, parent, right, leftAsChild)){
                mergeNodes(parent->key[parent->size - 1], newParent, parent, right);
            }
            else if(leftAsChild && !getKeyFromSibling(newParent, parent, left, leftAsChild)){
                mergeNodes(newParent->ptr[left]->key[newParent->ptr[left]->size-1], newParent, parent, left);
            }
        }
    }

};
mutex test;
void threadFunction(BPTree *tree){
    for(int i = 0; i < 100; i++){
        int a = rand() % 100 + 1;
        test.lock();
        tree->insertKey(a);
        cout << "Inserted: "<<a<<"\n";
        tree->printTree();
        test.unlock();
        this_thread::sleep_for(chrono::nanoseconds(500));
    }
}

int main(void) {
    BPTree tree;
    // In Baum Keys einfügen
    int dataInsert [17] = {5, 15, 25, 35, 45, 55, 40, 30, 20, 36, 42, 39, 52, 46, 38, 42, 42};
    for (int i : dataInsert){
        tree.insertKey(i);
        cout<<"Insert Key: "<<i<<"\n";
    }
    tree.printTree();
    // Aus Baum Keys entfernen
    int dataDelete[] = {5,42,30,15};
    for (int i : dataDelete){
        cout<< "Try to delete Key: "<<i<<"\n";
        tree.deleteKey(i);
    }
    tree.printTree();
    // Suchen, ob im Baum vorhanden
    int dataSearch[] = {42,30,55,100,40,25};
    for(int i : dataSearch){
        Node * result = tree.searchTree(i);
        if(result != nullptr){
            cout<<"Found key: " << i<<"\t";
            result->printNode();
            cout<<"\n\n";
        }
        else{
            cout<<"Key: "<< i << " not Found!\n\n";
        }
    }
    BPTree treeThread;
    //thread t1([&treeThread] { return threadFunction(&treeThread); });
    //thread t2([&treeThread] { return threadFunction(&treeThread); });
    //t1.join();
    //t2.join();
    threadFunction(&treeThread);
    threadFunction(&treeThread);
    treeThread.printTree();
    /*for(int i = 0; i < 10;i++){
        int  a = rand() % 100 + 1;
        treeThread.deleteKey(a);
    }
    treeThread.printTree()*/
    return 0;
}