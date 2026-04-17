# C String Hanlding

> Fixed the recursive format bug -> Problem was resuing the static ArgBuffer
> Adding ArgBuffer pool instead

**TODO**
- [x] The StorageOptions buffer is quite alot stack overhead -> Perf 
- [ ] Instead of exanding directly, just calculate length -> expand all together
- [ ] Implement some kind of stack like usage for recursive string formatting
    -> Just run one format at a time, and store it in a stack structure
    -> Then expand it all together
- [ ] Option for direct expansion into output buffer (no intermediate printing)
