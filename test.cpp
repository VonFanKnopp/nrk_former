Bitboards key = canonicalize(node.boardState);

auto it = visited.find(key);

if (it != visited.end() && it->second <= node.depth) {
    // A state reached at an equal or lower depth was already found.
    // Skip this node.
}
else {
    visited[key] = node.depth;
    // Continue exploring.
}