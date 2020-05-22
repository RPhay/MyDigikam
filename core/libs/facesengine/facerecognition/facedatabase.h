#ifndef FACEDATABASE_H
#define FACEDATABASE_H

namespace RecognitionTest
{

class FaceDatabase
{
public:
    FaceDatabase();
    ~FaceDatabase();

private:

    class Private;
    Private* d;
};

}

#endif // FACEDATABASE_H
