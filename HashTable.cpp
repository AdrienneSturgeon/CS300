//============================================================================
// Name        : HashTable.cpp
// Author      : Adrienne Sturgeon
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hash Table Structure for Course List, Ansi-style
//============================================================================

#include <algorithm>
#include <iostream>
#include <string> // atoi
#include <time.h>
#include <climits>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

//This will be the default size for the table for efficiency
const unsigned int DEFAULT_SIZE = 1000;

//Here I define a field which I will use for converting strings
double strToDouble(string str, char ch);

// Here I create a structure in order to hold information about the courses
struct Course {
    string courseId; // unique identifier
    string courseTitle;
    string coursePrerequisite;
    string courseDescription;
    
    //This is a simple constructor with course description having a default value based on how my code works
    Course() {
        courseDescription = 0.0;
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Here I define the structure to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        //This is the default structure
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // This is the structure if there is just a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // This is the structure if there is a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    //This is a vector to store the nodes for each of my courses
    vector<Node> nodes;

    //I set the hashtable size to a default size
    unsigned int sizeTable = DEFAULT_SIZE;

    unsigned int hash(int key);

    //Here I define the methods I will be using
public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void insertCourse(Course course);
    void printCourses();
    void deleteCourse(string courseId);
    Course printSpecificCourse(string courseId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Here I initialize the structure for my courses by resizing the table
    nodes.resize(sizeTable); 
}

/**
 * I use a constructor to specify the hash table size in order to make it more memory efficient
 */
HashTable::HashTable(unsigned int size) {

    //I use a lambda expression to set my local table size to the primary size
    this->sizeTable = size; // lambda
    nodes.resize(sizeTable); // I resize the tablet

}


/**
 * My Destructor
 */
HashTable::~HashTable() {
    //Here I erase the nodes
    nodes.erase(nodes.begin());
}

/**
 * I create an unsigent integer key in order to prevent incorrect results
 *
 * The parameter key is the key I am hashing
 * I then use return to return the calculated hash
 */
unsigned int HashTable::hash(int key) {
    //This is how I create a has value
    return key % sizeTable; // now I return key sizeTable
}

/**
 * This the insert course method
 *
 * I set the course object as the paramter to insert
 */
void HashTable::insertCourse(Course course) {

    unsigned key = hash(atoi(course.courseId.c_str()));  // Here I create the key which will be used for each course
    
    Node* oldNode = &(nodes.at(key)); // I set what will be the previous node to the current key node
 
    if (oldNode == nullptr) {  // If the node doe not exist then I create a new node with this key
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode)); // I set the new node to this key position in order
    }
    //Otherwise, if the new node is used I set the node to the end   
    else {
        if (oldNode->key == UINT_MAX) { // I set the old node key to the end and the node to point to null
            oldNode->key = key;
            oldNode->course = course;
            oldNode->next = nullptr;
        }
        else {  
            //Otherwise I find the next node
            while (oldNode->next != nullptr) {
                oldNode = oldNode->next;  
            }
        }
    }
}

/**
 * Print all of the course titles
 */
void HashTable::printCourses() {

    //I create a vector to store the course pointers
    vector<int> nodePointers;
    
    //I use a for loop to iterate over the courses
    for (unsigned int i = 0; i < nodes.size(); ++i) { 
        if (!nodes[i].course.courseTitle.empty()) {  //I add the course to the list if it isn't empty
            nodePointers.push_back(i); 
        }
    }

    //Now I sort the courses in alphanumeric order
    sort(nodePointers.begin(), nodePointers.end(), [&](int x, int y) { 
        return nodes[x].course.courseTitle < nodes[y].course.courseTitle;
        });
    for (int index : nodePointers) {  //Lastly I print the course list
        cout << nodes[index].course.courseTitle << endl;
    }
}

/*
 * This is the method to delete a course
 *
 * I set the courseId as the parameter to use to delete courses
 */
void HashTable::deleteCourse(string courseId) {

    unsigned key = hash(atoi(courseId.c_str())); // Here I create the key which will be used for each course
    
    nodes.erase(nodes.begin() + key); // Now I erase the node at that key location
}

/**
 * This is the method to print a specific course with its information and prerequisite
 *
 * I use the courseId to find the course
 */
Course HashTable::printSpecificCourse(string courseId) {

    //I create a course object to store course information
    Course course;

    unsigned key = hash(atoi(courseId.c_str())); // Here I create the key which will be used for each course

    Node* node = &(nodes.at(key)); //I set a node pointer for the node I will be looking for

    //If I find a node that matches the course ID I returne it
    if (node != nullptr && node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
        return node->course; 
    }

    //If I don't find the node I return an empty course
    if (node == nullptr || node->key == UINT_MAX) { 
        return course; 
    }
    
        
    //I use a while loop to search the course list and return it
    while (node != nullptr) { 
        if (node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) { // This is an if statement to compare the course key values
            return node->course; 
        }
        node = node->next; // I traverse to the next node
    }

    return course;
}

//============================================================================
// Static methods used for testing
//============================================================================

//Thi is the display course method to output the course information (used by the print specific course method)
void displayCourse(Course course) {
    cout <<"Title: " << course.courseTitle << " | Description: " << course.courseDescription << " | Prerequisite: "
            << course.coursePrerequisite << endl;
    return;
}

/**
 * This is the method I use to load courses from a csv file
 */
void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    //I initialize the CSV parser
    csv::Parser file = csv::Parser(csvPath);

    //Now I read the header for each row
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        //I use a for loop to iterate over each row
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            //I create a course object which will store each of the course values
            Course course;
            course.courseId = file[i][1];
            course.courseTitle = file[i][0];
            course.coursePrerequisite = file[i][2];
            course.courseDescription = file[i][3];

            //cout << "Item: " << course.courseTitle << ", Prerequisite: " << course.coursePrerequisite << ", Description: " << course.courseDescription << endl; testing

            // I add each course to the hash table
            hashTable->insertCourse(course);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * This is the function I use to convert a string to a double so that I can compare it. I got it from http://stackoverflow.com/a/24875936
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

//This is my main method where I display menus and take user decisions
int main(int argc, char* argv[]) {

    // I use a switch case to determine csv pathing with the course key
    string csvPath, courseKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        courseKey = "100"; // This is a default key I would use to search for a course
        break;
    case 3:
        csvPath = argv[1];
        courseKey = argv[2];
        break;
    default:
        csvPath = "ListofCourses.csv"; // This is the default path for the courses csv
        courseKey = "100"; 
    }

    // 
    clock_t ticks;

    //I make a hashtable to store the courses
    HashTable* tableOfCourses;

    Course course;
    tableOfCourses = new HashTable();
    
    int choice = 0;
    while (choice != 9) { // This is the menu which users will be able to choose from
        cout << "Menu:" << endl;
        cout << "  1. Load Courses From CSV" << endl; //This option loads the courses from the csv file
        cout << "  2. Print List of Courses" << endl; //this will call the method to print all courses in alphanumeric order
        cout << "  3. Print information for a course" << endl;    // This finds a course and prints its information
        cout << "  4. Remove a course" << endl;  //This is to remove a course
        cout << "  5. Add a Course" << endl;  //This is to add a course
        cout << "  9. Exit Program" << endl; // This will exit the program
        cout << "Enter a choice: ";
        cin >> choice;
        
        //I added input validation to make sure the program wouldn't crash if the wrong choice was entered
        while (cin.fail()) // this is a while loop to test a failed input
        {
            cin.clear();                                         // This clears any existing input information
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // This will ignore incorrect inputs
            cout << "That is not a choice please choose from the list:" << endl; // This prints an error message to let the user know they made a mistake
            cout << "Menu:" << endl;
            cout << "  1. Load Courses From CSV" << endl; //This option loads the courses from the csv file
            cout << "  2. Print List of Courses" << endl; //this will call the method to print all courses in alphanumeric order
            cout << "  3. Print information for a course" << endl;    // This finds a course and prints its information
            cout << "  4. Remove a course" << endl;  //This is to remove a course
            cout << "  5. Add a Course" << endl;  //This is to add a course
            cout << "  9. Exit Program" << endl; // This will exit the program
            cout << "Enter a choice: ";
            cin >> choice; //I take the user's choice 

        }
        
        //This is input validation for if the user does not enter the correct number
        if (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5 && choice != 9) { 
            cout << "That is not a choice please choose from the list but entering a number." << endl; 
            continue;
        }

        //Now I have a switch case for the user's choices
        switch (choice) {

        case 1:
            
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadCourses(csvPath, tableOfCourses);


            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            tableOfCourses->printCourses();
            break;

        case 3:
            ticks = clock();

            cout << "Please enter a course number without the letters:";
            cin >> courseKey;

            course = tableOfCourses->printSpecificCourse(courseKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!course.courseId.empty()) {
                displayCourse(course);
            } else {
                cout << "The course number " << courseKey << " was not found." << endl;
            }

            //Here I output the time it took to process
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;
        
        //This is the choice to remove a course
        case 4:
            cout << "Please enter a course number without the letters:";
            cin >> courseKey;
            tableOfCourses->deleteCourse(courseKey);
            break;

        //This choice allows the user the add a course, which I used for testing purposes
        case 5:  
            //I create a course object to store the course information
            Course course;

            //This is for the course ID
            cout << "Enter the course number after the letters:";  
            cin.ignore();
            getline(cin, course.courseId);

            //This is for the course title
            cout << "Enter course title: "; 
            getline(cin, course.courseTitle);

            //This is for the course prerequisites
            cout << "Enter course prerequisite: ";  
            getline(cin, course.coursePrerequisite);

            //This is for the course description
            cout << "Enter course description: "; 
            getline(cin, course.courseDescription);  

            //now I use the insert method to add the course to the hash table
            tableOfCourses->insertCourse(course);  

            //I also show the course so I can see what I added
            displayCourse(course);        

            break;
        }
       
    }

    cout << "Thank you for using this program :)" << endl;

    return 0;
}
