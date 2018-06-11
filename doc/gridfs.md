GridFS
======

Methods
-------

### gridfs:createFile([options])
Returns a new [GridFS file].

The following options are recognized as key-value pairs:

| Option      | Type            |
|-------------|-----------------|
| aliases     | [BSON document] |
| chunkSize   | integer         |
| contentType | string          |
| filename    | string          |
| md5         | string          |
| metadata    | [BSON document] |

### gridfs:createFileFrom(filename, [options])
Returns a new [GridFS file] with the contents of a local file `filename`. On error, returns `nil`
and the error message.

### gridfs:drop()
Drops `gridfs` and returns `true`. On error, returns `nil` and the error message.

### gridfs:find(query, [options])
Finds all files in `gridfs` that match `query` and returns a [GridFS file list].

### gridfs:findOne(query, [options])
Returns the first [GridFS file] in `gridfs` that matches `query` or `nil` if nothing was found.
On error, returns `nil` and the error message.

### gridfs:findOneByFilename(filename)
Returns the first [GridFS file] in `gridfs` that matches `filename` or `nil` if nothing was found.
On error, returns `nil` and the error message.

### gridfs:getChunks()
Returns a [Collection] handle that provides raw access to chunks of files in `gridfs`.

### gridfs:getFiles()
Returns a [Collection] handle that provides raw access to file metadata in `gridfs`.

### gridfs:removeByFilename(filename)
Removes all files in `gridfs` that match `filename` and returns `true`. On error, returns `nil` and
the error message.


[BSON document]: bson.md
[Collection]: collection.md
[GridFS file]: gridfsfile.md
[GridFS file list]: gridfsfilelist.md
