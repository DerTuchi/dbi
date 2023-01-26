#include <iostream>
#include <iterator>
#include <algorithm>
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
            if(cursor->size == order && cursor->key[order-1] == key && cursor->key[0] == key){
                cursor = parent->ptr[leftChild];
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
            //WICHTIGE STELLE MIT DEM NEWKEY
            newKey = virtualKey[(order + 1)/2];

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
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node *cursor = searchTree(key);
        Node *parent;
        bool condition = true;
        int leftSibling = 0, rightSibling = 0;
        if (cursor == nullptr){
            cout<<key<<" not in Tree";
            return;
        }
        cursor = root;
        for(int i = 0; i < cursor->size; i ++){
            if (cursor->key[i]==key){
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
                if (cursor->key[i] == key){
                    condition = false;
                }
            }
        }
        //Check for duplicate entries
        if (!cursor->leaf){
            for (int i = 1; i < cursor->size; i ++){
                for (int j = 0; j < cursor->ptr[i]->size-1; j++){
                    if(cursor->ptr[i]->key[j] == key){
                        cursor = cursor->ptr[i];
                        break;
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
            for (int j = i; j < cursor->size -1; j++){
                cursor->key[j] = cursor->key[j+1];
            }
            cursor->size --;
            // Wird die Min-Größe NICHT eingehalten muss nimmt man ein element vom direkten Nachbarn.
            if (cursor->size < threshhold){
                Node *leftChild = nullptr, *rightChild = nullptr;
                if(rightSibling <= order+1){
                    rightChild = parent->ptr[rightSibling];
                }
                if(leftSibling >= 0){
                    leftChild = parent->ptr[leftSibling];
                }
                //Rechtes Node ein Element nehmen
                if(rightChild != nullptr && rightChild->size > threshhold){
                    cursor->key[cursor->size] = rightChild->key[0];
                    cursor->size ++;
                    for (int i = 0; i < rightChild->size; i ++){
                        rightChild->key[i] = rightChild->key[i+1];
                    }
                    rightChild->size--;
                    parent->key[rightSibling - 1] = cursor->key[0];
                    parent->key[rightSibling] = rightChild->key[0];
                    return;
                }
                // Linkes Node ein element nehmen
                else if(leftChild != nullptr && leftChild->size > threshhold){
                    for(int i = cursor->size; i > 0; i --){
                        cursor->key[i] = cursor->key[i-1];
                    }
                    cursor->key[0] = leftChild->key[leftChild->size];
                    cursor->size ++;
                    leftChild->size --;
                    parent->key[leftSibling + 1] = cursor->key[0];
                    return;
                }
                // Wenn beides nicht geht, muss gemerged werden
                else{
                    if(rightSibling <= order+1){
                        mergeNodes(key, parent,cursor, rightSibling);
                    }
                    else{
                        mergeNodes(key, parent,cursor, leftSibling);
                    }
                }
            }
        }
        //TODO: Wenn es kein LEAF ist muss ich damit anders umgehen
        else{
            int parentKeyLocation=0;
            while(true){
                if(cursor->key[parentKeyLocation] == key){
                    break;
                }
                parentKeyLocation++;
            }
            parent = cursor;
            for (int j = 0; j < cursor->size; j++){
                if (key < cursor->key[j]){
                    cursor = cursor->ptr[j];
                    leftSibling = j-1;
                    rightSibling = j+1;
                    break;
                }
                if (j == cursor->size - 1){
                    cursor = cursor->ptr[j + 1];
                    leftSibling = j;
                    rightSibling = j+2;
                    break;
                }
            }
            bool parentFollowsLeaf = false;
            for (int j = 0; j < cursor->size; j ++) {
                if (cursor->leaf && cursor->key[j] == key) {
                    parentFollowsLeaf = true;
                }
            }
            if (parentFollowsLeaf){
                int cursorKeyLocation = 0;
                while(true){
                    if(cursor->key[cursorKeyLocation] == key){
                        break;
                    }
                    cursorKeyLocation ++;
                }
                for (int j = cursorKeyLocation; j < cursor->size - 1; j ++){
                    cursor->key[j] = cursor->key[j+1];
                }
                if (cursor->size > threshhold){
                    cursor->size --;
                    parent->key[parentKeyLocation] = cursor->key[0];
                }
                else if (cursor->size == threshhold){
                    int nodeTemp [order];
                    nodeTemp[0] = parent->ptr[leftSibling]->key[parent->ptr[leftSibling]->size-1];
                    for (int j = 1; j < cursor->size; j++){
                        nodeTemp[j] = cursor->key[j];
                    }
                    for (int j =0; j < cursor->size + 1; j++){
                        cursor->key[j] = nodeTemp[j];
                    }
                    parent->key[parentKeyLocation] = nodeTemp[0];
                    parent->ptr[leftSibling]->size --;
                }
            }
            else {
                Node * temp = cursor;
                condition = true;
                while (condition){
                    temp = cursor;
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
                        if (cursor->key[i] == key){
                            condition = false;
                        }
                    }
                }
                //Löschen in leaf und aufrücken der anderen Nodes
                int leafKeyLocation = 0;
                for(leafKeyLocation; leafKeyLocation < cursor->size; leafKeyLocation++){
                    if(cursor->key[leafKeyLocation] == key){
                        break;
                    }
                }
                for(int i = leafKeyLocation; i < cursor->size-1; i++){
                    cursor->key[i] = cursor->key[i+1];
                }
                cursor->size --;
                //Leafs zusammen fügen
                    int newSize = temp->ptr[0]->size + temp->ptr[1]->size;
                    int tempValues [newSize];
                    int tempValuesCount = 0;
                    for(tempValuesCount; tempValuesCount < temp->ptr[0]->size;tempValuesCount++){
                        tempValues[tempValuesCount] = temp->ptr[0]->key[tempValuesCount];
                    }
                    for(int i = 0; i < temp->ptr[1]->size; i++){
                        tempValues[i + tempValuesCount] = temp->ptr[1]->key[i];
                    }
                    for(int i = 0; i < newSize;i++){
                        cursor->key[i] = tempValues[i];
                    }
                    cursor->size = newSize;
                    for(int i = 1; i < temp->size+1;i++){
                        if (i == temp->size){
                            temp->ptr[i] = nullptr;
                            break;
                        }
                        else{
                            temp->ptr[i]->size = temp->ptr[i+1]->size;
                        }
                        for(int j = 0; j < temp->ptr[i]->size;j++){
                            temp->ptr[i]->key[j] = temp->ptr[i+1]->key[j];
                        }
                    }
                //Im parent den neuen Wert ändern
                parent->key[parentKeyLocation] = temp->key[0];
                for(int i = 0; i < temp->size-1; i++){
                    temp->key[i] = temp->key[i+1];
                }
                temp->size --;
                cursor = temp;
            }
            cout<<"Parent:";
            parent->printNode();
            cout<<"\nCursor:";
            cursor->printNode();
            cout<<"\n";
            leftSibling = parent->size - 1;
            rightSibling = parent->size;
            if (cursor->size < threshhold && parent->ptr[rightSibling]->size < threshhold){
                for (int i = 0; i < root->size; i ++){
                    if (root->key[i] == key){
                        Node *temp = parent->ptr[leftSibling];
                        temp->key[temp->size] = cursor->ptr[0]->key[0];
                        temp->size++;
                        temp->ptr[temp->size + 1] = cursor->ptr[0];
                        root = temp;
                        return;
                    }
                }
                Node *temp = parent->ptr[leftSibling];
                temp->key[temp->size] = parent->key[parent->size];
                temp->size++;
                temp->ptr[temp->size + 1] = cursor->ptr[0];
                root = temp;
            }
        }
    }

    void mergeNodes(int key, Node *parent, Node *cursor, int mergePartner){
        int threshhold = ((order/2) + 0.5) - 1;
        if(threshhold <= 0){
            threshhold = 1;
        }
        Node * child = parent->ptr[mergePartner];
        //child ist rechts
        if(key < child->key[0]){
            //Schauen ob der Parent beim Merge einen anderen Wert hat
            if(child->key[0] != parent->key[mergePartner - 1]){
                for(int i = child->size; i > 0; i --){
                    child->key[i] = child->key[i-1];
                }
                child->key[0] = child->key[mergePartner - 1];
                child->size++;
            }
            //Wenn es kein Leaf ist sollen auch die Pointer mit gemerged werden
            if(!cursor->leaf){
               for(int i = 0; i < child->size + 1; i++){
                   cursor->ptr[cursor->size + i] = child->ptr[i];
               }
            }
            //Mergen der beiden Nodes
            for(int i = 0; i < child->size; i ++){
                cursor->key[cursor->size + i] = child->key[i];
            }
        }
        //child ist links
        else{
            //Schauen ob der Parent beim Merge einen anderen Wert hat
            if(cursor->key[0] != parent->key[mergePartner]){
                for(int i = cursor->size; i > 0; i --){
                    cursor->key[i] = cursor->key[i-1];
                }
                cursor->key[0] = parent->key[mergePartner];
                cursor->size++;
            }
            //Wenn es kein Leaf ist sollen auch die Pointer mit gemerged werden
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
        //Anpassen des Parents beim Merge
        for(int i = mergePartner - 1; i < parent->size; i++){
            parent->key[i] = parent->key[i+1];
        }
        for(int  i = mergePartner; i < parent->size + 1; i ++){
            parent->ptr[i] = parent->ptr[i+1];
        }
        parent->size --;
        //Wenn Parent Root ist und gleich 0, wird cursor zum root
        if(parent == root && parent->size == 0){
            root = cursor;
            return;
        }
        //Rekursion, falls Parent den Threshhold nicht einhalten kann
        if (parent->size < threshhold && parent != root){
            Node *newParent = findParent(root, parent);
            int left, right;
            bool rightAsChild = true;
            for(int i = 0; i < newParent->size + 1; i ++){
                left = i - 1;
                right = i + 1;
                if(newParent->ptr[i] == parent){
                    break;
                }
                if(i == newParent->size){
                    left = i;
                    right = i + 2;
                    rightAsChild = false;
                }
            }
            if(rightAsChild){
                mergeNodes(parent->key[parent->size - 1], newParent, parent, right);
            }
            else{
                mergeNodes(parent->key[0], newParent, parent, left);
            }
        }
    }

};

int main(void) {
    BPTree tree;
    int data [17] = {5, 15, 25, 35, 45, 55, 40, 30, 20, 36, 42, 39, 52, 46, 38, 42, 42};
    for (int i : data){
        tree.insertKey(i);
    }
    tree.printTree();
    int dataDelete[] = {40};
    for (int i : dataDelete){
        tree.deleteKey(i);
        tree.printTree();
    }

    //Todo: Anschauen was passiert wenn ich 36 Lösche? Einfach mal alle ausprobieren. BEIM LÖSCHEN DES LINKEN CHILDS FUNKTIONOIERT ES NICHT!
    //tree.printTree();
    return 0;
}