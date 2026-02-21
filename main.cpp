#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstring>
#include <emscripten.h>

using namespace std;

// Shared buffer for results
vector<uint8_t> outputBuffer;

struct Node {
    uint8_t ch;
    int freq;
    Node *left, *right;
    // Initialize ch to 0 for internal nodes
    Node(uint8_t c, int f, Node* l = nullptr, Node* r = nullptr) 
        : ch(c), freq(f), left(l), right(r) {}
};

struct comp {
    bool operator()(Node* l, Node* r) {
        if (l->freq != r->freq) return l->freq > r->freq;
        // CRITICAL: Tie-breaker for identical frequencies
        return l->ch > r->ch; 
    }
};

Node* buildTree(unordered_map<uint8_t, int>& freq) {
    priority_queue<Node*, vector<Node*>, comp> pq;
    for (auto const& [ch, f] : freq) pq.push(new Node(ch, f));
    
    while (pq.size() > 1) {
        Node *l = pq.top(); pq.pop();
        Node *r = pq.top(); pq.pop();
        // The new internal node should take the smaller 'ch' 
        // value of its children to maintain deterministic sorting
        uint8_t minCh = (l->ch < r->ch) ? l->ch : r->ch;
        pq.push(new Node(minCh, l->freq + r->freq, l, r));
    }
    return pq.empty() ? nullptr : pq.top();
}

void encode(Node* root, string str, unordered_map<uint8_t, string> &huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) huffmanCode[root->ch] = str;
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void compress_text(const uint8_t* input, int length) {
    outputBuffer.clear();
    if (length == 0) return;

    unordered_map<uint8_t, int> freq;
    for (int i = 0; i < length; i++) freq[input[i]]++;

    // Header: Unique Chars
    uint32_t uniqueChars = freq.size();
    uint8_t temp[4];
    memcpy(temp, &uniqueChars, 4);
    for(int i=0; i<4; i++) outputBuffer.push_back(temp[i]);

    // Frequency Table
    for (auto const& [ch, f] : freq) {
        outputBuffer.push_back(ch);
        memcpy(temp, &f, 4);
        for(int i=0; i<4; i++) outputBuffer.push_back(temp[i]);
    }

    // Original Length
    uint32_t totalLen = length;
    memcpy(temp, &totalLen, 4);
    for(int i=0; i<4; i++) outputBuffer.push_back(temp[i]);

    Node* root = buildTree(freq);
    unordered_map<uint8_t, string> codes;
    encode(root, "", codes);

    string bitStream = "";
    for (int i = 0; i < length; i++) bitStream += codes[input[i]];

    uint8_t byte = 0;
    int bitCount = 0;
    for (char bit : bitStream) {
        byte = (byte << 1) | (bit == '1' ? 1 : 0);
        bitCount++;
        if (bitCount == 8) {
            outputBuffer.push_back(byte);
            byte = 0; bitCount = 0;
        }
    }
    if (bitCount > 0) outputBuffer.push_back(byte << (8 - bitCount));
}

EMSCRIPTEN_KEEPALIVE
void decompress_file(const uint8_t* input, int total_size) {
    outputBuffer.clear();
    if (total_size < 8) return;

    int pos = 0;
    uint32_t uniqueChars;
    memcpy(&uniqueChars, input + pos, 4);
    pos += 4;

    unordered_map<uint8_t, int> freq;
    for(uint32_t i = 0; i < uniqueChars; i++) {
        uint8_t ch = input[pos++];
        int f;
        memcpy(&f, input + pos, 4);
        pos += 4;
        freq[ch] = f;
    }

    uint32_t textLen;
    memcpy(&textLen, input + pos, 4);
    pos += 4;

    Node* root = buildTree(freq);
    Node* curr = root;
    uint32_t decodedCount = 0;

    for (int i = pos; i < total_size && decodedCount < textLen; i++) {
        for (int bit = 7; bit >= 0 && decodedCount < textLen; bit--) {
            curr = ((input[i] >> bit) & 1) ? curr->right : curr->left;
            if (curr && !curr->left && !curr->right) {
                outputBuffer.push_back(curr->ch);
                curr = root;
                decodedCount++;
            }
        }
    }
}

EMSCRIPTEN_KEEPALIVE
uint8_t* get_result_ptr() { return outputBuffer.data(); }

EMSCRIPTEN_KEEPALIVE
int get_result_size() { return outputBuffer.size(); }

}