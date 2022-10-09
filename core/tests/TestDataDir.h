#ifndef TEST_DATA_DIR_H
#define TEST_DATA_DIR_H

#include <QDir>

/** 
 * \brief Class that provides convenient access to test data directories and files.
 * 
 * When instantiated, the test-data directory is located dynamically; this algorithm works as long as
 * the current directory is the source root directory or any sub-directory.  After construction,
 * the function \ref isValid returns true if the test data was successfully located.
 */

class TestDataDir
{
public:
    /** True if the instance is correctly instantiated.
     * Valid means that the desired root directory was located. */
    bool isValid() { return _isValid; }

    /** Root directory of test data hierarchy. */
    QDir root() { return _testDataDir; }
    
    /** Path to any test file or directory, specified using relative path from root. */
    QString path(QString name) { return root().filePath(name); }

    /** Any test directory, specified using relative path from root. */
    QDir dir(QString relPath) { return QDir(path(relPath)); }

    /** Any test file, specified using relative path from root. */
    QFile file(QString name) { return QFile(path(name)); }
    
    /** Returns TestDataDir for the Digikam Test Data root directory.
     *  This provides access to all files in Digikam Test Data.
     */
    static TestDataDir TestDataRoot()
    {
        return TestDataDir();
    }

    /** Returns TestDataDir for a sub-tree of the Digikam Test Data.
     *  This provides access to files in the subtree.
     * 
     * \param subdirPath path of subdir, relative to the Digikam Test Data root
     */
    static TestDataDir TestData(QString subdirPath)
    {
        return TestDataDir(subdirPath);
    }
    
private:
    QDir _testDataDir;
    bool _isValid;

    static bool FindDirectoryUpwards(QDir dir, QString target, QDir& result)
    {
        while (dir.exists(target) == false)
        {
            if (!dir.cdUp()) return false;
        }

        if (!dir.cd(target)) return false;
        
        result = dir;
        return true;
    }
    
    void Initialize()
    {
        _isValid = FindDirectoryUpwards(QDir(), QString::fromUtf8("test-data"), _testDataDir);
    }
    
protected:
    /** Create */
    TestDataDir()
    {
        Initialize();
    }
        
    TestDataDir(QString subdirPath)
    {
        Initialize();
        if (!_isValid) return;
        
        _isValid = _testDataDir.exists(subdirPath);
        if (!_isValid) return;
        _testDataDir.cd(subdirPath);
    }
};

#endif
