# Flash Lookup Dictionary

A high-performance dictionary application built with C++ backend and modern web frontend. Delivers lightning-fast word searches with multiple definitions, auto-suggestions, and spell checking.

## Live Demo
- **Link**: [flashlookup.netlify.app](https://flashlookup.netlify.app/)

## Features
- **Ultra-fast search** - 30x faster than JavaScript implementations
- **Multiple definitions** - Up to 4 definitions per word
- **Auto-suggestions** - Real-time word completion
- **Spell checking** - Smart correction suggestions
- **151k+ words** - Comprehensive dictionary database


## Algorithms & Data Structures

### Core Search Algorithm
- **Hash Map Lookup**: O(1) average case with `std::unordered_map<string, vector<string>>`
- **Key Normalization**: Lowercase conversion for case-insensitive matching
- **Multiple Definitions**: Each word maps to vector of definitions
- **Design Choice**: Optimized from Trie to hash map for better maintainability

### Auto-Suggestion Engine
- **Linear Prefix Scan**: Iterates through `std::vector<string>` wordList
- **String Prefix Match**: Uses `substr()` comparison for prefix detection
- **Early Termination**: Breaks loop when limit reached
- **Complexity**: O(n) where n is dictionary size
- **Trade-off**: Simpler code vs Trie's O(k) performance

### Spell Correction System
- **Levenshtein Distance**: Dynamic programming for edit distance
- **Complexity**: O(m×n) for string lengths m,n
- **Distance Threshold**: Max 2 edits to reduce computation
- **Ranking**: Sort by distance, then alphabetically

### Memory Management
- **Dictionary**: `unordered_map<string, vector<string>>`
- **Word List**: Separate vector for iterations
- **String Reuse**: Minimize allocations

### Algorithm Evolution
- **Previous Version**: Trie-based implementation with O(k) suggestions (~10μs)
- **Current Version**: Hash map + linear scan for better maintainability (~260μs)
- **Performance Trade-off**: 26x slower suggestions but 50% less memory usage
- **Design Decision**: Prioritized code simplicity and maintainability over micro-optimizations

## Tech Stack

### Frontend
- **HTML5** - Semantic structure
- **CSS3** - Responsive design
- **JavaScript** - Pure vanilla JS
- **Deployment** - Netlify

### Backend (C++)
- **HTTP Server**: Custom C++ implementation
- **Dictionary Engine**: Optimized hash-based lookup
- **Data Storage**: CSV format for fast loading
- **Deployment**: Railway.app


## Project Architecture

1. **🌐 User Browser** → Opens flashlookup.netlify.app
2. **💻 Frontend** → Sends HTTP API calls  
3. **⚡ C++ Backend** → Processes requests on Railway
4. **🔍 Dictionary Logic** → Executes search algorithms
5. **📊 CSV Database** → Returns word definitions


## Performance Analysis

### Performance Metrics
| Operation          | Response Time     | Throughput            |
|--------------------|-------------------|-----------------------|
| Dictionary Loading | 175ms             | 151k words loaded     |
| Word Search        | 35ns              | ~28M searches/sec     |
| Auto-suggestions   | 0.26ms            | ~3.8k suggestions/sec |

### Memory Usage
- **JavaScript**: ~800MB for basic functionality
- **C++**: ~23MB with multiple definitions
- **Efficiency**: 97% less memory usage

### Scalability
- **Concurrent Users**: 5000+ (vs 50 with JavaScript)
- **Infrastructure Cost**: 90% reduction
- **Response Time**: Sub-millisecond search results

## File Structure

```
Flash-Lookup/
├── README.md
├── Dockerfile              # Railway deployment config
├── .railwayignore         # Deployment exclusions
├── server.cpp             # HTTP server implementation
├── dictionary.cpp         # Core dictionary logic
├── dictionary.h           # Header definitions
├── dictionary.csv         # Word database (151k entries)
├── index.html            # Web interface
├── main.js               # Frontend JavaScript
└── style.css             # Responsive styling
```

### Development Notes
- Keep response times under 50ms
- Ensure CORS compatibility


## Author
Built by Farhad Nuri - Focused on performance and user experience

---

*This project demonstrates the power of C++ for web backends while maintaining modern frontend practices.*
