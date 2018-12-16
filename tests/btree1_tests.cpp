////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Unit-тесты для B-деревьев
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.05.2017
///            This is a part of the course "Algorithms and Data Structures"
///            provided by  the School of Software Engineering of the Faculty
///            of Computer Science at the Higher School of Economics.
///
/// Gtest-based unit test.
/// The naming conventions imply the name of a unit-test module is the same as
/// the name of the corresponding tested module with _test suffix
///
///
////////////////////////////////////////////////////////////////////////////////




#include <gtest/gtest.h>


#include "btree.h"

/** \brief Путь к каталогу с рабочими тестовыми файлами. */
static const char* TEST_FILES_PATH = "../../out/";



using namespace xi;


/** \brief Тестовый класс для тестирования открытых интерфейсов B-tree. */
class BTreeTest : public ::testing::Test {
public:
    //BTreeTest()
    //    : _dumper(DUMP_EVENTLOG_PUB_FN, DUMP_IMGS_PUB_PATH)
    //{
    //}

public:
    //static const int STRUCT2_SEQ[];
    //static const int STRUCT2_SEQ_NUM;
    std::string& getFn(const char* fn)
    {
        _fn = TEST_FILES_PATH;
        _fn.append(fn);
        return _fn;
    }

protected:
    std::string _fn;        ///< Имя файла
    //RBTreeDefDumper<int, std::less<int>> _dumper;

    ///** \brief Выводить в формате GraphViz. */
    //RBTreeGvDumper<int, std::less<int>> _gvDumper;
}; // class RBTreePubTest






TEST_F(BTreeTest, Simplest)
{
    //EXPECT_TRUE(tree.isEmpty());
}


TEST_F(BTreeTest, Order1)
{
    std::string fn(TEST_FILES_PATH);    // имя файла
    fn.append("order1.xibt");

    FileBaseBTree bt(2, 10, nullptr, fn);        // без компаратора!
    EXPECT_EQ(10, bt.getRecSize());

    EXPECT_EQ(2, bt.getOrder());
    EXPECT_EQ(3, bt.getMaxKeys());
    EXPECT_EQ(1, bt.getMinKeys());
    EXPECT_EQ(30, bt.getKeysSize());
    EXPECT_EQ(32, bt.getCursorsOfs()); // 30 + 2
    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48

    EXPECT_EQ(1, bt.getLastPageNum());
    EXPECT_EQ(1, bt.getRootPageNum());


    //EXPECT_TRUE(tree.isEmpty());
}


TEST_F(BTreeTest, AllocPage1)
{
    std::string fn(TEST_FILES_PATH);
    fn.append("AllocPage1.xibt");


    FileBaseBTree bt(2, 10, nullptr, fn);       // без компаратора!
    FileBaseBTree::PageWrapper wp(&bt);


    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    bt.allocPage(wp, 1, true);  //true);
    EXPECT_EQ(2, bt.getLastPageNum());

    bt.allocPage(wp, 2, false);  //false);
    EXPECT_EQ(3, bt.getLastPageNum());

    bt.allocPage(wp, 3, true);  //true);
    EXPECT_EQ(4, bt.getLastPageNum());
}



TEST_F(BTreeTest, ReadPage1)
{
    std::string fn(TEST_FILES_PATH);
    fn.append("ReadPage1.xibt");


    FileBaseBTree bt(2, 10, nullptr, fn);   // без компаратора!
    FileBaseBTree::PageWrapper wp(&bt);


    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    bt.allocPage(wp, 1, true);  //true);
    EXPECT_EQ(2, bt.getLastPageNum());

    bt.allocPage(wp, 2, false);  //false);
    EXPECT_EQ(3, bt.getLastPageNum());

    bt.allocPage(wp, 3, true);  //true);
    EXPECT_EQ(4, bt.getLastPageNum());

    // читаем странички (1-я теперь под корень!)
    //bt.readWorkPage(2);
    wp.readPage(2);
    EXPECT_TRUE(wp.isLeaf());
    EXPECT_EQ(1, wp.getKeysNum());

    //bt.readWorkPage(4);
    wp.readPage(4);
    EXPECT_TRUE(wp.isLeaf());
    EXPECT_EQ(3, wp.getKeysNum());

    //bt.readWorkPage(3);
    wp.readPage(3);
    EXPECT_FALSE(wp.isLeaf());
    EXPECT_EQ(2, wp.getKeysNum());
}


// тестируем запись модифицированных страничек
TEST_F(BTreeTest, WritePage1)
{
    std::string& fn = getFn("WritePage1.xibt");


    FileBaseBTree bt(2, 10, nullptr, fn);   // без компаратора!
    FileBaseBTree::PageWrapper wp(&bt);

    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    bt.allocPage(wp, 1, true);  //true);
    EXPECT_EQ(2, bt.getLastPageNum());

    bt.allocPage(wp, 2, false);  //false);
    EXPECT_EQ(3, bt.getLastPageNum());


    // читаем странички
    //bt.readWorkPage(2);
    wp.readPage(2);
    EXPECT_TRUE(wp.isLeaf());
    EXPECT_EQ(1, wp.getKeysNum());



    // модифицируем страничку 2
    wp.setLeaf(false);                // была истина
    *(wp.getKey(0)) = 'A';            // нумерация ключей — с нуля!!!
    //bt.writeWorkPage(2);
    wp.writePage();

    //bt.readWorkPage(3);
    wp.readPage(3);
    EXPECT_FALSE(wp.isLeaf());
    EXPECT_EQ(2, wp.getKeysNum());

    // модифицируем страничку 3
    wp.setKeyNum(3);                  // сделали три ключа на пробу!
    *(wp.getKey(1)) = 'B';
    //bt.writeWorkPage(3);
    wp.writePage();


    // еще раз читаем страницы
    //bt.readWorkPage(2);
    wp.readPage(2);
    EXPECT_FALSE(wp.isLeaf());        // теперь это должен быть не лист
    EXPECT_EQ(1, wp.getKeysNum());
    EXPECT_EQ('A', *(wp.getKey(0)));

    // еще раз читаем страницы
    //bt.readWorkPage(3);
    wp.readPage(3);
    EXPECT_FALSE(wp.isLeaf());
    EXPECT_EQ(3, wp.getKeysNum());    // теперь тут три ключа должно быть!
    EXPECT_EQ('B', *(wp.getKey(1)));

}


// тестируем запись модифицированных страничек через рабочие страницы
TEST_F(BTreeTest, WritePage2)
{
    std::string& fn = getFn("WritePage2.xibt");


    FileBaseBTree bt(2, 10, nullptr, fn);   // без компаратора!
    FileBaseBTree::PageWrapper wp(&bt);

    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    bt.allocPage(wp, 1, true);  //true);
    EXPECT_EQ(2, bt.getLastPageNum());

    bt.allocPage(wp, 2, false);  //false);
    EXPECT_EQ(3, bt.getLastPageNum());


    FileBaseBTree::PageWrapper wp1(&bt);

    // читаем и модифицируем страничку 2
    wp1.readPage(2);
    EXPECT_TRUE(wp1.isLeaf());
    EXPECT_EQ(1, wp1.getKeysNum());

    wp1.setLeaf(false);                // была истина
    *(wp1.getKey(0)) = 'A';            // нумерация ключей — с нуля!!!
    wp1.writePage();


    // читаем и модифицируем страничку 3
    wp1.readPage(3);
    EXPECT_FALSE(wp1.isLeaf());
    EXPECT_EQ(2, wp1.getKeysNum());

    wp1.setKeyNum(3);                   // сделали три ключа на пробу!
    *(wp1.getKey(1)) = 'B';             // нумерация ключей — с нуля!!!
    wp1.writePage();

    FileBaseBTree::PageWrapper wp2(&bt);

    // еще раз читаем страницы
    wp2.readPage(2);
    EXPECT_FALSE(wp2.isLeaf());
    EXPECT_EQ(1, wp2.getKeysNum());
    EXPECT_EQ('A', *(wp2.getKey(0)));

    wp2.readPage(3);
    EXPECT_FALSE(wp2.isLeaf());
    EXPECT_EQ(3, wp2.getKeysNum());
    EXPECT_EQ('B', *(wp2.getKey(1)));
}

// тестирует создание файла с последующим переотркрытием на том же объекте!
TEST_F(BTreeTest, ReopenFile1)
{
    std::string& fn = getFn("ReopenFile1.xibt");

    FileBaseBTree bt(2, 10, nullptr, fn);   // без компаратора!
    FileBaseBTree::PageWrapper wp(&bt);

    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    bt.allocPage(wp, 1, true);  //true);
    EXPECT_EQ(2, bt.getLastPageNum());

    bt.allocPage(wp, 2, false);  //false);
    EXPECT_EQ(3, bt.getLastPageNum());


    // читаем странички
    //bt.readWorkPage(2);
    wp.readPage(2);
    EXPECT_TRUE(wp.isLeaf());
    EXPECT_EQ(1, wp.getKeysNum());

    //bt.readWorkPage(3);
    wp.readPage(3);
    EXPECT_FALSE(wp.isLeaf());
    EXPECT_EQ(2, wp.getKeysNum());

    bt.close();


    // открываем заново
    bt.open(fn);    // , nullptr);
    EXPECT_EQ(10, bt.getRecSize());

    EXPECT_EQ(2, bt.getOrder());
    EXPECT_EQ(3, bt.getMaxKeys());
    EXPECT_EQ(1, bt.getMinKeys());
    EXPECT_EQ(30, bt.getKeysSize());
    EXPECT_EQ(32, bt.getCursorsOfs()); // 30 + 2
    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48

    EXPECT_EQ(3, bt.getLastPageNum());
    EXPECT_EQ(1, bt.getRootPageNum());

}

// тестирует создание файла с последующим переотркрытием в другом объекте!
TEST_F(BTreeTest, ReopenFile2)
{
    std::string& fn = getFn("ReopenFile2.xibt");

    FileBaseBTree bt(2, 10, nullptr, fn);   // без компаратора!
    FileBaseBTree::PageWrapper wp(&bt);

    EXPECT_EQ(48, bt.getNodePageSize()); // 32 + 4 * 4 = 48
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    bt.allocPage(wp, 1, true);  //true);
    EXPECT_EQ(2, bt.getLastPageNum());

    bt.allocPage(wp, 2, false);  //false);
    EXPECT_EQ(3, bt.getLastPageNum());


    // читаем странички
    //bt.readWorkPage(2);
    wp.readPage(2);
    EXPECT_TRUE(wp.isLeaf());
    EXPECT_EQ(1, wp.getKeysNum());

    //bt.readWorkPage(3);
    wp.readPage(3);
    EXPECT_FALSE(wp.isLeaf());
    EXPECT_EQ(2, wp.getKeysNum());

    bt.close();


    // создаем новое дерево и открываем
    FileBaseBTree bt2(fn, nullptr);   // без компаратора!
    EXPECT_EQ(10, bt2.getRecSize());

    EXPECT_EQ(2, bt2.getOrder());
    EXPECT_EQ(3, bt2.getMaxKeys());
    EXPECT_EQ(1, bt2.getMinKeys());
    EXPECT_EQ(30, bt2.getKeysSize());
    EXPECT_EQ(32, bt2.getCursorsOfs()); // 30 + 2
    EXPECT_EQ(48, bt2.getNodePageSize()); // 32 + 4 * 4 = 48

    EXPECT_EQ(3, bt2.getLastPageNum());
    EXPECT_EQ(1, bt2.getRootPageNum());

}

TEST_F(BTreeTest, WorkPages1)
{
    std::string& fn = getFn("WorkPages1.xibt");

    FileBaseBTree bt(2, 10, nullptr, fn);   // без компаратора!
    FileBaseBTree::PageWrapper wp1(&bt);
    EXPECT_EQ(1, bt.getLastPageNum());

    // добавим пару страничек
    wp1.allocPage(1, true);
    EXPECT_EQ(2, bt.getLastPageNum());
    EXPECT_FALSE(wp1.isFull());


    wp1.allocPage(3, false);
    EXPECT_EQ(3, bt.getLastPageNum());
    EXPECT_TRUE(wp1.isFull());
}


TEST_F(BTreeTest, KeysNCursors1)
{
    std::string& fn = getFn("KeysNCursors1.xibt");

    FileBaseBTree bt(2, 10, nullptr, fn);
    FileBaseBTree::PageWrapper wp1(&bt);
    //FileBaseBTree::PageWrapper wp2(&bt);

    wp1.allocPage(1, false);
    *(wp1.getKey(0)) = 'A';
    EXPECT_EQ(nullptr, wp1.getKey(1));
    //EXPECT_THROW(*(wp1.getKey(1)) = 'B', DerivedClassException);

    wp1.setCursor(0, 1);
    wp1.setCursor(1, 2);
    EXPECT_THROW(wp1.setCursor(2, 42), std::invalid_argument);
    wp1.writePage();

    //EXPECT_E
}

TEST_F(BTreeTest, KeysNCursors2)
{
    std::string& fn = getFn("KeysNCursors2.xibt");


    FileBaseBTree bt(3, 2, nullptr, fn);
    FileBaseBTree::PageWrapper wp1(&bt);

    wp1.allocPage(3, false);
    *((UShort*)wp1.getKey(0)) = 0x0201;
    *((UShort*)wp1.getKey(1)) = 0x0207;
    *((UShort*)wp1.getKey(2)) = 0x0208;

    wp1.setCursor(0, 0xF0F0F0F0);           // отладочные бетолковые значения
    wp1.setCursor(1, 0xF1F1F1F1);
    wp1.setCursor(2, 0xF2F2F2F2);
    wp1.setCursor(3, 0xF3F3F3F3);

    wp1.writePage();

    FileBaseBTree::PageWrapper wp2(&bt);
    wp2.allocPage(5, false);  // nLeaf);
    *((UShort*)wp2.getKey(0)) = 0x0302;
    *((UShort*)wp2.getKey(1)) = 0x0303;
    *((UShort*)wp2.getKey(2)) = 0x0304;
    *((UShort*)wp2.getKey(3)) = 0x0305;
    *((UShort*)wp2.getKey(4)) = 0x0306;

    wp2.setCursor(0, 0xB0B0B0B0);           // отладочные бетолковые значения
    wp2.setCursor(1, 0xB1B1B1B1);
    wp2.setCursor(2, 0xB2B2B2B2);
    wp2.setCursor(3, 0xB3B3B3B3);
    wp2.setCursor(4, 0xB4B4B4B4);
    wp2.setCursor(5, 0xB5B5B5B5);


    wp2.writePage();

    // уст. курсор
    wp1.setCursor(1, wp2.getPageNum());         // ссылаемся на 3 страницу
    wp1.writePage();
}



TEST_F(BTreeTest, Split1)
{
    std::string& fn = getFn("Split1.xibt");

    // 2 байта на ключ, от 3х до 5 ключей
    FileBaseBTree bt(3, 2, nullptr, fn);
    FileBaseBTree::PageWrapper parent(&bt);

    parent.allocPage(3, false);
    *((UShort*)parent.getKey(0)) = 0x0201;
    *((UShort*)parent.getKey(1)) = 0x0207;
    *((UShort*)parent.getKey(2)) = 0x0208;

    parent.setCursor(0, 0xF0F0F0F0);           // отладочные бестолковые значения
    parent.setCursor(1, 0xF1F1F1F1);
    parent.setCursor(2, 0xF2F2F2F2);
    parent.setCursor(3, 0xF3F3F3F3);

    parent.writePage();

    FileBaseBTree::PageWrapper child(&bt);
    child.allocPage(5, false);  // nLeaf);
    *((UShort*)child.getKey(0)) = 0x0302;
    *((UShort*)child.getKey(1)) = 0x0303;
    *((UShort*)child.getKey(2)) = 0x0304;
    *((UShort*)child.getKey(3)) = 0x0305;
    *((UShort*)child.getKey(4)) = 0x0306;

    child.setCursor(0, 0xB0B0B0B0);           // отладочные бестолковые значения
    child.setCursor(1, 0xB1B1B1B1);
    child.setCursor(2, 0xB2B2B2B2);
    child.setCursor(3, 0xB3B3B3B3);
    child.setCursor(4, 0xB4B4B4B4);
    child.setCursor(5, 0xB5B5B5B5);


    child.writePage();

    // уст. курсор
    parent.setCursor(1, child.getPageNum());         // ссылаемся на 3 страницу
    parent.writePage();


    // before
    EXPECT_EQ(bt.getLastPageNum(), 3); // две наши страницы плюс одна пустая
    EXPECT_EQ(parent.getKeysNum(), 3);
    EXPECT_EQ(child.getKeysNum(), 5);

    // выполняем собственно сплит
    // split between 0x0201 and 0x0207
    parent.splitChild(1);                          // курсор на 3 страницу

    // after
    FileBaseBTree::PageWrapper newChild(&bt);
    newChild.readPageFromChild(parent, 2);
    child.readPageFromChild(parent, 1);

    EXPECT_EQ(bt.getLastPageNum(), 4);
    EXPECT_EQ(parent.getKeysNum(), 4);
    EXPECT_EQ(*((UShort*)parent.getKey(0)), 0x0201);
    EXPECT_EQ(*((UShort*)parent.getKey(1)), 0x0304); // inserted
    EXPECT_EQ(*((UShort*)parent.getKey(2)), 0x0207);
    EXPECT_EQ(*((UShort*)parent.getKey(3)), 0x0208);

    // check cursors!! They should be shifted.
    EXPECT_EQ(parent.getCursor(0), 0xF0F0F0F0);
    EXPECT_EQ(parent.getCursor(1), child.getPageNum());
    EXPECT_EQ(parent.getCursor(2), newChild.getPageNum());
    EXPECT_EQ(parent.getCursor(3), 0xF2F2F2F2);
    EXPECT_EQ(parent.getCursor(4), 0xF3F3F3F3);


    EXPECT_EQ(child.getKeysNum(), 2);
    EXPECT_EQ(newChild.getKeysNum(), 2);

    EXPECT_EQ(*((UShort*)child.getKey(0)), 0x0302);
    EXPECT_EQ(*((UShort*)child.getKey(1)), 0x0303);

    EXPECT_EQ(*((UShort*)newChild.getKey(0)), 0x0305);
    EXPECT_EQ(*((UShort*)newChild.getKey(1)), 0x0306);
}

TEST_F(BTreeTest, Split2)
{
    std::string& fn = getFn("Split2.xibt");


    FileBaseBTree bt(2, 1, nullptr, fn);
    FileBaseBTree::PageWrapper parent(&bt);

    parent.allocPage(2, false);
    *(parent.getKey(0)) = 0x01;
    *(parent.getKey(1)) = 0x07;


    parent.setCursor(0, 0xF0F0F0F0);           // отладочные бестолковые значения
    parent.setCursor(1, 0xF1F1F1F1);
    parent.setCursor(2, 0xF2F2F2F2);

    parent.writePage();

    FileBaseBTree::PageWrapper child(&bt);
    child.allocPage(3, false);  // nLeaf);
    *(child.getKey(0)) = 0x08;
    *(child.getKey(1)) = 0x09;
    *(child.getKey(2)) = 0x0A;


    child.setCursor(0, 0xB0B0B0B0);           // отладочные бестолковые значения
    child.setCursor(1, 0xB1B1B1B1);
    child.setCursor(2, 0xB2B2B2B2);
    child.setCursor(3, 0xB3B3B3B3);


    child.writePage();

    // уст. курсор
    parent.setCursor(2, child.getPageNum());     // ссылаемся на 3 страницу крайним правым курсором
    parent.writePage();

    EXPECT_EQ(parent.getKeysNum(), 2);

    // выполняем собственно сплит
    parent.splitChild(2);                      // 2-й курсор на 3 страницу


    EXPECT_EQ(parent.getKeysNum(), 3);
    EXPECT_EQ(*parent.getKey(0), 0x01);
    EXPECT_EQ(*parent.getKey(1), 0x07);
    EXPECT_EQ(*parent.getKey(2), 0x09); // inserted

    FileBaseBTree::PageWrapper newChild(&bt);
    newChild.readPageFromChild(parent, 3);
    child.readPageFromChild(parent, 2);

    EXPECT_EQ(child.getKeysNum(), 1);
    EXPECT_EQ(newChild.getKeysNum(), 1);

    EXPECT_EQ(*child.getKey(0), 0x08);
    EXPECT_EQ(*newChild.getKey(0), 0x0A);

}


// простой сравниватель байт
struct ByteComparator : public BaseBTree::IComparator {
    virtual bool compare(const Byte* lhv, const Byte* rhv, UInt sz) override
    {
        int notEqualBytePos = 0;
        for (int i = 0; i < sz; ++i)
        {
            if (lhv[i] != rhv[i]) {
                notEqualBytePos = i;
                break;
            }
        }

        return lhv[notEqualBytePos] < rhv[notEqualBytePos];
    }

    // простейшая реализация — побайтное сравнение
    virtual bool isEqual(const Byte* lhv, const Byte* rhv, UInt sz) override
    {
        for (UInt i = 0; i < sz; ++i)
            if (*lhv != *rhv)
                return false;

        return true;
    }



};


TEST_F(BTreeTest, InsertNonFull1)
{
    std::string& fn = getFn("InsertNonFull1.xibt");

    ByteComparator comparator;

    FileBaseBTree bt(2, 1, &comparator, fn);
    FileBaseBTree::PageWrapper parent(&bt);

    parent.allocPage(2, false);
    *(parent.getKey(0)) = 0x05;
    *(parent.getKey(1)) = 0x11;


    parent.writePage();

    FileBaseBTree::PageWrapper leaf1(&bt);
    leaf1.allocPage(2, true);  // nLeaf);
    *(leaf1.getKey(0)) = 0x01;
    *(leaf1.getKey(1)) = 0x03;
    leaf1.writePage();

    FileBaseBTree::PageWrapper leaf2(&bt);
    leaf2.allocPage(2, true);  // nLeaf);
    *(leaf2.getKey(0)) = 0x07;
    *(leaf2.getKey(1)) = 0x09;
    leaf2.writePage();

    FileBaseBTree::PageWrapper leaf3(&bt);
    leaf3.allocPage(2, true);  // nLeaf);
    *(leaf3.getKey(0)) = 0x13;
    *(leaf3.getKey(1)) = 0x15;
    leaf3.writePage();

    // уст. курсор
    parent.setCursor(0, leaf1.getPageNum());
    parent.setCursor(1, leaf2.getPageNum());
    parent.setCursor(2, leaf3.getPageNum());
    parent.writePage();


    // вставляем 1 в "верхний" узел
    // (на самом деле вставляется в первый лист)
    Byte k = 0x02;
    parent.insertNonFull(&k);

    leaf1.readPage(leaf1.getPageNum()); // обновляем
    EXPECT_EQ(leaf1.getKeysNum(), 3);
    EXPECT_EQ(*(leaf1.getKey(0)), 0x01);
    EXPECT_EQ(*(leaf1.getKey(1)), 0x02);
    EXPECT_EQ(*(leaf1.getKey(2)), 0x03);

    // теперь первый лист заполнен
    // внутри должен произойти сплит
    k = 0x02;
    parent.insertNonFull(&k);

    leaf1.readPage(leaf1.getPageNum()); // обновляем лист
    parent.readPage(parent.getPageNum()); // обновляем отца

    // проверка на сплит
    EXPECT_EQ(parent.getKeysNum(), 3);
    EXPECT_EQ(*parent.getKey(0), 0x02); // inserted
    EXPECT_EQ(*parent.getKey(1), 0x05);
    EXPECT_EQ(*parent.getKey(2), 0x11);


    FileBaseBTree::PageWrapper left(&bt);
    left.readPageFromChild(parent, 0);
    FileBaseBTree::PageWrapper right(&bt);
    right.readPageFromChild(parent, 1);

    EXPECT_EQ(left.getPageNum(), leaf1.getPageNum());

    EXPECT_EQ(left.getKeysNum() + right.getKeysNum(), 3);
    EXPECT_EQ(abs(left.getKeysNum() - right.getKeysNum()), 1); // l=2 r=1 or l=1 r=2

    // непонятно куда она пойдёт.. но точно не все три в одну.
    if (left.getKeysNum() == 1) {
        EXPECT_EQ(*left.getKey(0), 0x01);
        EXPECT_EQ(*right.getKey(0), 0x02);
        EXPECT_EQ(*right.getKey(1), 0x03);
        std::cout << "case 1" << std::endl;
    } else {
        EXPECT_EQ(*left.getKey(0), 0x01);
        EXPECT_EQ(*left.getKey(1), 0x02);
        EXPECT_EQ(*right.getKey(0), 0x03);
        std::cout << "case 2" << std::endl;
    }
}


TEST_F(BTreeTest, Insert1)
{
    std::string& fn = getFn("Insert1.xibt");

    ByteComparator comparator;
    FileBaseBTree bt(2, 1, &comparator, fn);


    Byte k = 0x03;
    bt.insert(&k);

    k = 0x02;
    bt.insert(&k);

    k = 0x01;
    bt.insert(&k);

    FileBaseBTree::PageWrapper root(&bt);
    root.readPage(bt.getRootPageNum());

    EXPECT_EQ(root.getKeysNum(), 3);
    EXPECT_EQ(*root.getKey(0), 0x01);
    EXPECT_EQ(*root.getKey(1), 0x02);
    EXPECT_EQ(*root.getKey(2), 0x03);
}

TEST_F(BTreeTest, Insert2)
{
    std::string& fn = getFn("Insert2.xibt");

    ByteComparator comparator;
    FileBaseBTree bt(2, 1, &comparator, fn);


    Byte k = 0x03;
    bt.insert(&k);

    k = 0x02;
    bt.insert(&k);

    k = 0x01;
    bt.insert(&k);

    k = 0x04;
    // create new root, split old by 0x02, insert 0x04 to the right child.
    bt.insert(&k);

    FileBaseBTree::PageWrapper root(&bt);
    root.readPage(bt.getRootPageNum());
    EXPECT_EQ(root.getKeysNum(), 1);
    EXPECT_EQ(*root.getKey(0), 0x02);

    FileBaseBTree::PageWrapper left(&bt);
    left.readPage(root.getCursor(0));
    EXPECT_EQ(left.getKeysNum(), 1);
    EXPECT_EQ(*left.getKey(0), 0x01);

    FileBaseBTree::PageWrapper right(&bt);
    right.readPage(root.getCursor(1));
    EXPECT_EQ(right.getKeysNum(), 2);
    EXPECT_EQ(*right.getKey(0), 0x03);
    EXPECT_EQ(*right.getKey(1), 0x04);
}


TEST_F(BTreeTest, InsertAndSearch)
{
    std::string& fn = getFn("InsertAndSearch.xibt");

    ByteComparator comparator;
    FileBaseBTree bt(2, 1, &comparator, fn);


    Byte els[] = { 0x01, 0x11, 0x09, 0x05, 0x07, 0x03, 0x04, 0x08, 0x00, 0x06 };
    for (int i = 0; i < sizeof(els) / sizeof(els[0]); ++i)
    {
        Byte& el = els[i];

        EXPECT_EQ(bt.search(&el), nullptr);
        bt.insert(&el);
        EXPECT_EQ(*bt.search(&el), el);
    }
}

TEST_F(BTreeTest, InsertUShort)
{
    std::string& fn = getFn("InsertUShort.xibt");

    ByteComparator comparator;
    FileBaseBTree bt(2, 2, &comparator, fn);

    UShort els[] = { 0x2222, 0x1111, 0x0000, 0x5555, 0x7777, 0x3333, 0x4444, 0x8888, 0x9999, 0x6666 };
    for (UShort el : els)
    {
        Byte* byteArr = (Byte*)(&el);

        EXPECT_EQ(bt.search(byteArr), nullptr);
        bt.insert(byteArr);
        EXPECT_EQ(*((UShort*)bt.search(byteArr)), el);
    }
}



TEST_F(BTreeTest, InsertRepeating)
{
    std::string& fn = getFn("InsertRepeating.xibt");

    ByteComparator comparator;
    FileBaseBTree bt(2, 1, &comparator, fn);

    Byte els[] = { 0x01, 0x02, 0x03, 0x01, 0x01, 0x01, 0x03, 0x04, 0x02, 0x01, 0x01, 0x01 };
    for (int i = 0; i < sizeof(els) / sizeof(els[0]); ++i)
    {
        Byte& el = els[i];
        bt.insert(&el);
    }


    Byte one = 0x01;
    Byte two = 0x02;
    Byte three = 0x03;
    Byte four = 0x04;

    std::list<Byte*> found;

    found.clear();
    bt.searchAll(&one, found);
    EXPECT_EQ(found.size(), 6);
    for (Byte* item : found) EXPECT_EQ(*item, one);

    found.clear();
    bt.searchAll(&two, found);
    EXPECT_EQ(found.size(), 2);
    for (Byte* item : found) EXPECT_EQ(*item, two);

    found.clear();
    bt.searchAll(&three, found);
    EXPECT_EQ(found.size(), 2);
    for (Byte* item : found) EXPECT_EQ(*item, three);

    found.clear();
    bt.searchAll(&four, found);
    EXPECT_EQ(found.size(), 1);
    for (Byte* item : found) EXPECT_EQ(*item, four);
}


TEST_F(BTreeTest, InsertRepeatingUShort)
{
    std::string& fn = getFn("InsertRepeatingUShort.xibt");

    ByteComparator comparator;
    FileBaseBTree bt(2, 2, &comparator, fn);

    UShort els[] = { 0x1234, 0x7777, 0x1234, 0x7777, 0x1234, };
    for (UShort el : els)
    {
        Byte* byteArr = (Byte*)(&el);
        bt.insert(byteArr);
    }

    UShort ord = 0x1234;
    UShort seven = 0x7777;

    std::list<Byte*> found;

    found.clear();
    bt.searchAll((Byte*)(&ord), found);
    EXPECT_EQ(found.size(), 3);
    for (Byte* item : found) EXPECT_EQ(*(UShort*)item, ord);

    found.clear();
    bt.searchAll((Byte*)(&seven), found);
    EXPECT_EQ(found.size(), 2);
    for (Byte* item : found) EXPECT_EQ(*(UShort*)item, seven);
}




