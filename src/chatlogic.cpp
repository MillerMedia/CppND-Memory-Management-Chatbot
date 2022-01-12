#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"

using std::make_unique;
using std::unique_ptr;

ChatLogic::ChatLogic()
{
    //// STUDENT CODE
    ////

  	// ChatLogic should have no ownership. Declaring object here (in the constructor) would put ownership of this ChatBot instance
  	// in the scope of ChatLogic
  
    // create instance of chatbot
    //_chatBot = new ChatBot("../images/chatbot.png");

    // add pointer to chatlogic so that chatbot answers can be passed on to the GUI
    //_chatBot->SetChatLogicHandle(this);

    ////
    //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic()
{
    //// STUDENT CODE
    ////

    // delete chatbot instance
    //delete _chatBot;

    // delete all nodes
  // Now that nodes contain unique pointers, they will be automatically deallocated when out of scope
  
    /*for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    {
        delete *it;
    }*/

    // delete all edges
    /*for (auto it = std::begin(_edges); it != std::end(_edges); ++it)
    {
        delete *it;
    }*/

    ////
    //// EOF STUDENT CODE
}

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    // find all occurences for current node
    auto token = tokens.begin();
    while (true)
    {
        token = std::find_if(token, tokens.end(), [&tokenID](const std::pair<std::string, std::string> &pair) { return pair.first == tokenID;; });
        if (token != tokens.end())
        {
            element.AddToken(token->second); // add new keyword to edge
            token++;                         // increment iterator to next element
        }
        else
        {
            break; // quit infinite while-loop
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    // load file with answer graph elements
    std::ifstream file(filename);

    // check for file availability and process it line by line
    if (file)
    {
        // loop over all lines in the file
        std::string lineStr;
        while (getline(file, lineStr))
        {
            // extract all tokens from current line
            tokenlist tokens;
            while (lineStr.size() > 0)
            {
                // extract next token
                int posTokenFront = lineStr.find("<");
                int posTokenBack = lineStr.find(">");
                if (posTokenFront < 0 || posTokenBack < 0)
                    break; // quit loop if no complete token has been found
                std::string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

                // extract token type and info
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != std::string::npos)
                {
                    std::string tokenType = tokenStr.substr(0, posTokenInfo);
                    std::string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

                    // add token to vector
                    tokens.push_back(std::make_pair(tokenType, tokenInfo));
                }

                // remove token from current line
                lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
            }

            // process tokens for current line
            auto type = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "TYPE"; });
            if (type != tokens.end())
            {
                // check for id
                auto idToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "ID"; });
                if (idToken != tokens.end())
                {
                    // extract id from token
                    int id = std::stoi(idToken->second);

                    // node-based processing
                    if (type->second == "NODE")
                    {
                        //// STUDENT CODE
                        ////

                        // check if node with this ID exists already
                      // Took me way too long to add the ampersand in front of the variable (and likewise below). I'm not entirely sure why, would love some clarification on this
                      // in the review. Is this because the ampersand passes the address value of the node through to then be able to use the GetID() function and if we don't have
                      // the & it is trying to run function GetID() on a unique_ptr object?
                        auto newNode = std::find_if(
                          _nodes.begin(), _nodes.end(), [&id](unique_ptr<GraphNode> &node) {
                            return node->GetID() == id;
                          }
                        );

                        // create new element if ID does not yet exist
                        if (newNode == _nodes.end())
                        {
                          // In the lesson, basically learned never to use 'new' if you can avoid it
                          // A great SO answer on this topic here: https://stackoverflow.com/a/37514601/975592
                            _nodes.emplace_back(make_unique<GraphNode>(id));
                            newNode = _nodes.end() - 1; // get iterator to last element

                            // add all answers to current node
                            AddAllTokensToElement("ANSWER", tokens, **newNode);
                        }

                        ////
                        //// EOF STUDENT CODE
                    }

                    // edge-based processing
                    if (type->second == "EDGE")
                    {
                        //// STUDENT CODE
                        ////

                        // find tokens for incoming (parent) and outgoing (child) node
                        auto parentToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "PARENT"; });
                        auto childToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "CHILD"; });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                          // get iterator on incoming and outgoing node via ID search
                          // Added new lines and indents to make more readable while editing
                          auto parentNode = std::find_if(
                            _nodes.begin(), _nodes.end(), [&parentToken](unique_ptr<GraphNode> &node) {
                              return node->GetID() == std::stoi(parentToken->second);
                            }
                          );
                          
                          auto childNode = std::find_if(
                            _nodes.begin(), _nodes.end(), [&childToken](unique_ptr<GraphNode> &node) {
                              return node->GetID() == std::stoi(childToken->second);
                            }
                          );

                            // create new edge
                          // Revisiting on task 4. Graph edges objects are now unique pointers in the _childEdges vector when owned by GraphNode
                            unique_ptr<GraphEdge> edge = make_unique<GraphEdge>(id);
                            
                          	// Since child node is a unique pointer owned by ChatLogic, we need to get the value of child node and parent node and pass them through to the GraphEdge object rather
                          	// than pass through the actual pointer
                          	edge->SetChildNode((*childNode).get());                            
                          	edge->SetParentNode((*parentNode).get());
                            
                          // Use get() here instead of move since we still need the object existing after this function and are just storing its value in the _edges vector
                          	//_edges.push_back(edge.get());

                            // find all keywords for current node
                            AddAllTokensToElement("KEYWORD", tokens, *edge);

                            // store reference in child node and parent node
                          
                          // Parent node is non-unique; child node is unique. So we will store the get() value in the child node and then move the entire unique pointer to the child node
                            (*childNode)->AddEdgeToParentNode(edge.get());
                            (*parentNode)->AddEdgeToChildNode(std::move(edge));
                        }

                        ////
                        //// EOF STUDENT CODE
                    }
                }
                else
                {
                    std::cout << "Error: ID missing. Line is ignored!" << std::endl;
                }
            }
        } // eof loop over all lines in the file

        file.close();

    } // eof check for file availability
    else
    {
        std::cout << "File could not be opened!" << std::endl;
        return;
    }

    //// STUDENT CODE
    ////

    // identify root node
    GraphNode *rootNode = nullptr;
    for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    {
        // search for nodes which have no incoming edges
        if ((*it)->GetNumberOfParents() == 0)
        {

            if (rootNode == nullptr)
            {
              // Same as above; need to get the value
              // Perhaps this is incorrect? Should I be using other move semantics (i.e. std::move()?). I tried and it failed...
                rootNode = (*it).get(); // assign current node to root
            }
            else
            {
                std::cout << "ERROR : Multiple root nodes detected" << std::endl;
            }
        }
    }

  	// Instantiate local ChatBot.
  // This will release the object once it falls out of scope and allow it to more easily be moved through the application
  	ChatBot LocalChatbot = ChatBot("../images/chatbot.png");
  	
  	// Hands off new local class to the ChatLogic _chatbot variable for use
  	//SetChatbotHandle(&_chatbot);
  	LocalChatbot.SetChatLogicHandle(this);
    // add chatbot to graph root node
    LocalChatbot.SetRootNode(rootNode);
  
  	// Before moving, set ChatLogic on Chatbot so it knows it's association before moving to other places
  /*
  I still have some confusion on this. This works when passing 'this' (i.e. the current ChatLogic object) but the void function on Chatbot
  takes a pointer (*ChatLogic). When I try '&this' or '*this' it doesn't work. I figured I was supposed to pass the pointer to the object,
  not the object itself.
  */
  	
    rootNode->MoveChatbotHere(std::move(LocalChatbot));
  
    ////
    //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}
