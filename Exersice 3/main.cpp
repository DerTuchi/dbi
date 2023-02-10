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
    mutex mutexTree;
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
                    if (i == cursor->size -1){
                        cursor = cursor->ptr[i + 1];
                        leftChild = i;
                        break;
                    }
                }
            }
            /*  Wenn noch Platz im Leaf ist und ein Wert größer im Leaf ist, wird für Value platzt gemacht im Node, indem
             *  die anderen aufrücken und dann an der freien stelle die Value eingefügt wird.*/
            Node *temp = cursor;
            while(cursor != root && leftChild > -1){
                if(parent->ptr[leftChild]->size < order && temp->key[0] == key){
                    cursor = parent->ptr[leftChild];
                    break;
                }
                temp = parent->ptr[leftChild];
                leftChild--;
            }
            if(cursor->key[0] == key && cursor->size == order){
                for(int i = parent->size; i > 0;i--){
                    if(parent->ptr[i]->size < order && key > parent->ptr[i]->key[parent->ptr[i]->size-1]) cursor = parent->ptr[i];
                }
            }
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

        int newKey = 0;
        if (cursor->size < order) {
            int i = 0;
            while (key > cursor->key[i] && i < cursor->size){
                i++;
            }
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
            if(virtualPtrCopy[order]->key[virtualPtrCopy[order]->size-1] < child->key[child->size-1]) i = order;
            else{
                while (key > virtualKey[i] && i < order)i++;
                if(key == virtualKey[0]) i = 1;
            }
            for (int j = order + 1; j > i; j--) {
                virtualKey[j] = virtualKey[j - 1];
                virtualKeyCopy[j] = virtualKeyCopy[j - 1];
            }
            virtualKey[i] = key;
            virtualKeyCopy[i] = key;

            //WICHTIGE STELLE MIT DEM NEWKEY
            newKey = virtualKeyCopy[(order + 1)/2];

            for (int j = order + 2; j > i + 1; j--) {
                virtualPtr[j] = virtualPtr[j - 1];
            }
            virtualPtr[i + 1] = child;

            cursor->size = (order + 1) / 2;
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
        Node *cursor = new Node;
        cursor = root;
        while(!cursor->leaf){
            cursor = cursor->ptr[0];
            maxDepth ++;
        }
        while(currentDepth <= maxDepth){
            nodesPerDepth(root, 0, currentDepth);
                cout<<"\n\n\n";
                currentDepth ++;
        }
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
        cout<< "Try to delete Key: "<<key<<"\n";
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node *cursor = searchTree(key);
        Node *parent = nullptr;
        bool condition = true;
        int leftSibling = 0, rightSibling = 0;
        if (cursor == nullptr){
            cout<<key<<" not in Tree\n";
            return;
        }
        cursor = root;
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
            if(rightSibling <= parent->size){
                if(!getKeyFromSibling(parent, cursor, rightSibling, false)){
                    mergeNodes(parent,cursor, rightSibling, true);
                }
            }
            else if(leftSibling >= 0){
                if(!getKeyFromSibling(parent, cursor, leftSibling, true)){
                    mergeNodes(parent,cursor, leftSibling, false);
                }

            }
        }
    }

    void mergeNodes(Node *parent, Node *cursor, int mergePartner, bool sideIsRight){
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node * child = parent->ptr[mergePartner];
        //child ist rechts
        if(sideIsRight){
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
            for(int parentKeyLocation = mergePartner -1; parentKeyLocation < parent->size; parentKeyLocation ++){
                parent->key[parentKeyLocation] = parent->key[parentKeyLocation+1];
            }
            for(int i = mergePartner; i < parent->size + 1; i ++){
                if(i == parent->size){
                    parent->ptr[i] = nullptr;
                    break;
                }
                parent->ptr[i] = parent->ptr[i+1];
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
            for(int i = mergePartner; i < parent->size + 1; i ++){
                if(i == parent->size){
                    parent->ptr[i] = nullptr;
                     break;
                }
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

        //Löschen des childs PS:Macht irgendwie alles kaputt
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
                mergeNodes(newParent, parent, right, true);
            }
            else if(leftAsChild && !getKeyFromSibling(newParent, parent, left, leftAsChild)){
                mergeNodes(newParent, parent, left, false);
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
        if(!leftSibling && child->size > threshhold){
            // Falls der Parent einen anderen Wert hat, muss dieser beim Klauen des nachbarn berücksichtigt werden
            if(parent->key[sibling-1] != child->key[0]){
                cursor->key[cursor->size] = parent->key[sibling-1];
            }
            // Andernfalls kann man die Werte einfach Klauen und Löschen
            else{
                cursor->key[cursor->size] = child->key[0];
            }
            for (int i = child->size - 1; i > 0; i --){
                child->key[i] = child->key[i-1];
            }
            if(!cursor->leaf){
                for(int i = child->size; i > 0; i --){
                    child->ptr[i] = child->ptr[i-1];
                }
                cursor->ptr[cursor->size + 1] = child->ptr[0];
                child->ptr[child->size] = nullptr;
            }
            child->size--;
            cursor->size ++;
            parent->key[sibling - 1] = child->key[0];
            return true;
        }
            // Linkes Node ein element nehmen
        else if(leftSibling && child->size > threshhold){
            // Falls der Parent einen anderen Wert hat, muss dieser beim Klauen des nachbarn berücksichtigt werden
            for(int i = 0; i < cursor->size; i ++){
                cursor->key[i] = cursor->key[i+1];
            }
            if(!cursor->leaf){
                for(int i = cursor->size + 1; i > 0; i --){
                    cursor->ptr[i] = cursor->ptr[i-1];
                }
                cursor->ptr[0] = child->ptr[child->size];
                child->ptr[child->size] = nullptr;
            }
            if(parent->key[sibling] != cursor->key[0]){
                cursor->key[0] = parent->key[sibling];
                parent->key[sibling] = child->key[child->size];
            }
            // Andernfalls kann man die Werte einfach Klauen und Löschen
            else{
                cursor->key[0] = child->key[child->size-1];
                parent->key[sibling] = cursor->key[0];
            }
            cursor->size ++;
            child->size--;
            return true;
        }
        return false;
    }

};
void threadFunction(BPTree *tree, int seed, int thread){
    int t = time(0);
    srand(t + seed);
    for(int i = 0; i < 100; i++){
        int a = rand() % 100 + 1;
        tree->mutexTree.lock();
        tree->insertKey(a);
        tree->mutexTree.unlock();
    }
    this_thread::sleep_for(chrono::seconds(2));
    for(int i = 0; i < 10; i++){
        int a = rand() % 100 +1;
        tree->mutexTree.lock();
        tree->printTree();
        tree->deleteKey(a);
        tree->mutexTree.unlock();
        this_thread::sleep_for(chrono::nanoseconds(2000));
    }
}

int main(void) {
    BPTree tree;
    // In Baum Keys einfügen
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    cout<<"\t\t\tInsert Part\n";
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    int dataInsert [17] = {5, 15, 25, 35, 45, 55, 40, 30, 20, 36, 42, 39, 52, 46, 38, 42, 42};
    for (int i : dataInsert){
        tree.insertKey(i);
        cout<<"Insert Key: "<<i<<"\n";
    }
    tree.printTree();
    // Aus Baum Keys entfernen
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    cout<<"\t\t\tDelete Part\n";
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    int dataDelete[] = {5,42,30,15};
    for (int i : dataDelete){
        tree.deleteKey(i);
    }
    tree.printTree();
    // Suchen, ob im Baum vorhanden
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    cout<<"\t\t\tSearch Part\n";
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
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
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    cout<<"\t\t\tThreading Part\n";
    cout<<"-----------------------------------------------------------------------------------------------------------\n";
    BPTree treeThread;
    thread t1([&treeThread] { return threadFunction(&treeThread, 3*10^5+1, 1); });
    thread t2([&treeThread] { return threadFunction(&treeThread, 10, 2); });
    thread t3([&treeThread] { return threadFunction(&treeThread, 7*10^5+1, 3); });
    thread t4([&treeThread] { return threadFunction(&treeThread, 2*10^5+1, 4); });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    treeThread.printTree();
    return 0;
}