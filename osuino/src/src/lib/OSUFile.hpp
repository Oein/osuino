class StringParser {
public:
    IStringType content;
    int cursor = 0;
    int base64ToNumber(const IStringType base64) {
        const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int num = 0;
        int power = 1;
    
        for (int i = 0; i < base64.length(); i++) {
            if(base64[i] == '_') {
                continue;
            }
            int index = -1;
            for(int j = 0; j < base.length(); ++j) {
                if (base[j] == base64[i]) {
                    index = j;
                    break;
                }
            }
            if (index != -1) {
                num += index * power;
                power *= 64;
            } else {
                printf("Invalid base64 character(file int): %c, parsing %s\n", base64[i], base64.c_str());
                return -1; // Return an error code for invalid input
            }
        }
    
        return num;
    }
    
    unsigned long base64ToLargeNumber(const IStringType base64) {
        const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        unsigned long num = 0;
        unsigned long power = 1;
    
        for (int i = 0; i < base64.length(); i++) {
            if(base64[i] == '_') {
                continue;
            }
            int index = -1;
            for(int j = 0; j < base.length(); ++j) {
                if (base[j] == base64[i]) {
                    index = j;
                    break;
                }
            }
            if (index != -1) {
                num += index * power;
                power *= 64;
            } else {
                printf("Invalid base64 character(file long): %c, parsing %s\n", base64[i], base64.c_str());
                return -1; // Return an error code for invalid input
            }
        }
    
        return num;
    }

    StringParser(IStringType content) : content(content) {}
    IStringType parseString(int length) {
        // get length characters from cursor
        IStringType str = subString(content, cursor, length);
        cursor += length;
        // parse that str into number
        int num = base64ToNumber(str);
        // read num characters from cursor
        IStringType result = subString(content, cursor, num);
        cursor += num;
        return result;
    }

    IStringType getStringLength(int length) {
        IStringType str = subString(content, cursor, length);
        cursor += length;
        return str;
    }

    bool hasMore() {
        return cursor < content.length();
    }
    unsigned long left() {
        return content.length() - cursor;
    }
};

class OsuNote
{
public:
    int time;
    int endtime;
};

class OsuMetaFile
{
public:
    unsigned long id;
    unsigned long beatsetid;
    IStringType title;
    IStringType artist;
    IStringType version;
    int colChunksCount[4];
    int colNotesCount[4];

    void parse(StringParser &parser)
    {
        beatsetid = parser.base64ToLargeNumber(parser.getStringLength(6));
        id = parser.base64ToLargeNumber(parser.getStringLength(6));
        title = parser.parseString(2);
        artist = parser.parseString(2);
        version = parser.parseString(2);
        for (int i = 0; i < 4; i++)
        {
            colChunksCount[i] = parser.base64ToNumber(parser.getStringLength(2));
        }
        for(int i = 0; i < 4; i++) {
            colNotesCount[i] = parser.base64ToNumber(parser.getStringLength(3));
        }
    }
};

class OsuChunkedMap
{
public:
    IStringType id;
    int chunksCount[4];
    int chunksLoadedFrom[4];
    int chunksLoadedTo[4];
    int colNotesCount[4];
    IVector<OsuNote> notes[4];

    void unloadAll() {
        for (int i = 0; i < 4; i++)
        {
            chunksLoadedFrom[i] = -1;
            chunksLoadedTo[i] = -1;
            notes[i].clear();
        }
    }

    void initialize(unsigned long id) {
#ifdef _PRINTF_MAP_LOAD_INFO_
        printf("Loading map with id %s\n", ul2string(id).c_str());
#endif
        for (int i = 0; i < 4; i++)
        {
            chunksLoadedFrom[i] = -1;
            chunksLoadedTo[i] = -1;
            notes[i].clear();
        }
        this->id = ul2string(id);

        StringParser parser = StringParser(fsapi.get("map-" + this->id + ".meta"));
        OsuMetaFile meta;
        meta.parse(parser);

        for (int i = 0; i < 4; i++)
        {
            chunksCount[i] = meta.colChunksCount[i];
            colNotesCount[i] = meta.colNotesCount[i];
            loadChunk(i, 0);
        }
    }

    void loadChunk(int col, int chunk) {
        if (chunksLoadedFrom[col] <= chunk && chunk <= chunksLoadedTo[col]) {
#ifdef _PRINTF_MAP_LOAD_INFO_
            printf("Chunk %d for column %d already loaded\n", chunk, col);
#endif
            return;
        }
#ifdef _PRINTF_MAP_LOAD_INFO_
        printf("Loading chunk %d for column %d\n", chunk, col);
#endif

        StringParser parser = StringParser(fsapi.get("chunk-" + this->id + "-" + int2string(col) + "-" + int2string(chunk) + ".chunk"));
        while (parser.hasMore())
        {
            OsuNote note;
            note.time = parser.base64ToNumber(parser.getStringLength(4));
            char nowChar = parser.content[parser.cursor];
            note.endtime = 0;
            if(nowChar != '@') {
                note.endtime = parser.base64ToNumber(parser.getStringLength(4));
            } else parser.cursor++;
            notes[col].push_back(note);
        }

        if (chunksLoadedFrom[col] == -1)
        {
            chunksLoadedFrom[col] = chunk;
        } else if(chunksLoadedFrom[col] > chunk) {
            chunksLoadedFrom[col] = chunk;
        }

        if (chunksLoadedTo[col] == -1)
        {
            chunksLoadedTo[col] = chunk;
        } else if(chunksLoadedTo[col] < chunk) {
            chunksLoadedTo[col] = chunk;
        }
    }

    void unload(int col, int leastRendered) {
        int neededFrom = leastRendered / 100;
        if(neededFrom > chunksLoadedFrom[col]) {
            // remove 100 items
            int removeCount = 100;
            if(neededFrom < chunksLoadedTo[col]) {
                removeCount = neededFrom - chunksLoadedFrom[col];
            }

            notes[col].erase(notes[col].begin(), notes[col].begin() + removeCount);
            chunksLoadedFrom[col]++;
#ifdef _PRINTF_MAP_LOAD_INFO_
            printf("Unloading %d notes from column %d\n", removeCount, col);
#endif
        }
    }

    OsuNote get(int col, int index) {
        int indexChunk = index / 100;
        if(chunksLoadedTo[col] < indexChunk) {
            loadChunk(col, indexChunk);
        }
        return notes[col].data[index - chunksLoadedFrom[col] * 100];
    }
};