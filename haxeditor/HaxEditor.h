
#ifndef HAXEDITOR__H_
#define HAXEDITOR__H_

#include <string>

/*!
 * The base class for a haxeditor view of a file
 *
 * Basic file operations and undo stacks are handled in the FAL, this
 * class deals with the interpretation and modification of the data
 */
class HaxEditorBase
{
public:

    bool openFile(const std::string& /*filename*/)
    {
        return false;
    }
};

#endif // HAXEDITOR__H_
