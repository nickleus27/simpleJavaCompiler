import unittest
from subprocess import run
from os import remove
from os import path
from os import system
from os import listdir
import filecmp

class Test_sJava_Files(unittest.TestCase):
    sjc_exe = "src/build/./sjc"
    exe = "."
    dir = "tests"
    
    def setup(self):
        if not path.isfile(self.sjc_exe):
            src = "src"
            build = "build"
            buildPath = path.join(src, build)
            system("(cd " + buildPath + " && make)")
    
    def teardown(self):
        src = "src"
        build = "build"
        buildPath = path.join(src, build)
        if len([name for name in listdir(buildPath) if path.isfile(path.join(buildPath, name))]) > 1:
            system("(cd " + buildPath + " && make clean)")

    def test_test0(self):
        self.teardown()
        self.setup()
        test0 = path.join(self.dir, "test0.sjava")
        sjs = path.join(self.dir, "test0.sjava.s")
        sjo = path.join(self.dir, "test0.sjava.o")
        sjout = path.join(self.dir, "test0.sjava.out")
        tmp0 = path.join(self.dir, "temp0.txt")
        test_txt = path.join(self.dir, "test0.txt")
        exitStatus = run([self.sjc_exe, test0]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):  
            with open(tmp0, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp0):
            self.assertTrue(filecmp.cmp(tmp0, test_txt))
            remove(tmp0)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test1(self):
        self.setup()
        test1 = path.join(self.dir, "test1.sjava")
        sjs = path.join(self.dir, "test1.sjava.s")
        sjo = path.join(self.dir, "test1.sjava.o")
        sjout = path.join(self.dir, "test1.sjava.out")
        tmp1 = path.join(self.dir, "temp1.txt")
        test_txt = path.join(self.dir, "test1.txt")
        exitStatus = run([self.sjc_exe, test1]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp1, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp1):
            self.assertTrue(filecmp.cmp(tmp1, test_txt))
            remove(tmp1)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test2(self):
        self.setup()
        test2 = path.join(self.dir, "test2.sjava")
        sjs = path.join(self.dir, "test2.sjava.s")
        sjo = path.join(self.dir, "test2.sjava.o")
        sjout = path.join(self.dir, "test2.sjava.out")
        tmp2 = path.join(self.dir, "temp2.txt")
        test_txt = path.join(self.dir, "test2.txt")
        exitStatus = run([self.sjc_exe, test2]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp2, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp2):
            self.assertTrue(filecmp.cmp(tmp2, test_txt))
            remove(tmp2)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test2_1(self):
        self.setup()
        test2_1 = path.join(self.dir, "test2-1.sjava")
        sjs = path.join(self.dir, "test2-1.sjava.s")
        sjo = path.join(self.dir, "test2-1.sjava.o")
        sjout = path.join(self.dir, "test2-1.sjava.out")
        tmp2_1 = path.join(self.dir, "temp2-1.txt")
        test_txt = path.join(self.dir, "test2-1.txt")
        exitStatus = run([self.sjc_exe, test2_1]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp2_1, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp2_1):
            self.assertTrue(filecmp.cmp(tmp2_1, test_txt))
            remove(tmp2_1)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test3(self):
        self.setup()
        test3 = path.join(self.dir, "test3.sjava")
        sjs = path.join(self.dir, "test3.sjava.s")
        sjo = path.join(self.dir, "test3.sjava.o")
        sjout = path.join(self.dir, "test3.sjava.out")
        tmp3 = path.join(self.dir, "temp3.txt")
        test_txt = path.join(self.dir, "test3.txt")
        exitStatus = run([self.sjc_exe, test3]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp3, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp3):
            self.assertTrue(filecmp.cmp(tmp3, test_txt))
            remove(tmp3)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test4(self):
        self.setup()
        test4 = path.join(self.dir, "test4.sjava")
        sjs = path.join(self.dir, "test4.sjava.s")
        sjo = path.join(self.dir, "test4.sjava.o")
        sjout = path.join(self.dir, "test4.sjava.out")
        tmp4 = path.join(self.dir, "temp4.txt")
        test_txt = path.join(self.dir, "test4.txt")
        exitStatus = run([self.sjc_exe, test4]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp4, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp4):
            self.assertTrue(filecmp.cmp(tmp4, test_txt))
            remove(tmp4)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test4_1(self):
        self.setup()
        test4_1 = path.join(self.dir, "test4-1.sjava")
        sjs = path.join(self.dir, "test4-1.sjava.s")
        sjo = path.join(self.dir, "test4-1.sjava.o")
        sjout = path.join(self.dir, "test4-1.sjava.out")
        tmp4_1 = path.join(self.dir, "temp4-1.txt")
        test_txt = path.join(self.dir, "test4-1.txt")
        exitStatus = run([self.sjc_exe, test4_1]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp4_1, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp4_1):
            self.assertTrue(filecmp.cmp(tmp4_1, test_txt))
            remove(tmp4_1)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test5(self):
        self.setup()
        test5 = path.join(self.dir, "test5.sjava")
        sjs = path.join(self.dir, "test5.sjava.s")
        sjo = path.join(self.dir, "test5.sjava.o")
        sjout = path.join(self.dir, "test5.sjava.out")
        tmp5 = path.join(self.dir, "temp5.txt")
        test_txt = path.join(self.dir, "test5.txt")
        exitStatus = run([self.sjc_exe, test5]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp5, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp5):
            self.assertTrue(filecmp.cmp(tmp5, test_txt))
            remove(tmp5)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test6(self):
        self.setup()
        test6 = path.join(self.dir, "test6.sjava")
        sjs = path.join(self.dir, "test6.sjava.s")
        sjo = path.join(self.dir, "test6.sjava.o")
        sjout = path.join(self.dir, "test6.sjava.out")
        tmp6 = path.join(self.dir, "temp6.txt")
        test_txt = path.join(self.dir, "test6.txt")
        if path.isfile(test6):
            exitStatus = run([self.sjc_exe, test6]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile(sjout):
                with open(tmp6, 'w') as outfile:
                    run(path.join(self.exe, sjout), stdout=outfile)
                    outfile.close()
            if path.isfile(tmp6):
                self.assertTrue(filecmp.cmp(tmp6, test_txt))
                remove(tmp6)
            else:
                self.assertTrue(False)
            if path.isfile(sjo):
                remove(sjo)
            if path.isfile(sjout):
                remove(sjout)
            if path.isfile(sjs):
                remove(sjs)

    def test_test7(self):
        self.setup()
        test7 = path.join(self.dir, "test7.sjava")
        sjs = path.join(self.dir, "test7.sjava.s")
        sjo = path.join(self.dir, "test7.sjava.o")
        sjout = path.join(self.dir, "test7.sjava.out")
        tmp7 = path.join(self.dir, "temp7.txt")
        test_txt = path.join(self.dir, "test7.txt")
        exitStatus = run([self.sjc_exe, test7]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(tmp7, 'w') as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(tmp7):
            self.assertTrue(filecmp.cmp(tmp7, test_txt))
            remove(tmp7)
        else:
            self.assertTrue(False)
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test8(self):
        self.setup()
        test8 = path.join(self.dir, "test8.sjava")
        sjs = path.join(self.dir, "test8.sjava.s")
        sjo = path.join(self.dir, "test8.sjava.o")
        sjout = path.join(self.dir, "test8.sjava.out")
        tmp8 = path.join(self.dir, "temp8.txt")
        test_txt = path.join(self.dir, "test8.txt")
        if path.isfile(test8):
            exitStatus = run([self.sjc_exe, test8]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile(sjout):
                with open(tmp8, 'w') as outfile:
                    run(path.join(self.exe, sjout), stdout=outfile)
                    outfile.close()
            if path.isfile(tmp8):
                self.assertTrue(filecmp.cmp(tmp8, test_txt))
                remove(tmp8)
            else:
                self.assertTrue(False)
            if path.isfile(sjo):
                remove(sjo)
            if path.isfile(sjout):
                remove(sjout)
            if path.isfile(sjs):
                remove(sjs)

    def test_test9(self):
        self.setup()
        test9 = path.join(self.dir, "test9.sjava")
        sjs = path.join(self.dir, "test9.sjava.s")
        sjo = path.join(self.dir, "test9.sjava.o")
        sjout = path.join(self.dir, "test9.sjava.out")
        tmp9 = path.join(self.dir, "temp9.txt")
        test_txt = path.join(self.dir, "test9.txt")
        if path.isfile(test9):
            exitStatus = run([self.sjc_exe, test9]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile(sjout):
                with open(tmp9, 'w') as outfile:
                    run(path.join(self.exe, sjout), stdout=outfile)
                    outfile.close()
            if path.isfile(tmp9):
                self.assertTrue(filecmp.cmp(tmp9, test_txt))
                remove(tmp9)
            else:
                self.assertTrue(False)
            if path.isfile(sjo):
                remove(sjo)
            if path.isfile(sjout):
                remove(sjout)
            if path.isfile(sjs):
                remove(sjs)

    def test_test10(self):
        self.setup()
        test10 = path.join(self.dir, "test10.sjava")
        sjs = path.join(self.dir, "test10.sjava.s")
        sjo = path.join(self.dir, "test10.sjava.o")
        sjout = path.join(self.dir, "test10.sjava.out")
        tmp10 = path.join(self.dir, "temp10.txt")
        test_txt = path.join(self.dir, "test10.txt")
        if path.isfile(test10):
            exitStatus = run([self.sjc_exe, test10]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile(sjout):
                with open(tmp10, 'w') as outfile:
                    run(path.join(self.exe, sjout), stdout=outfile)
                    outfile.close()
            if path.isfile(tmp10):
                self.assertTrue(filecmp.cmp(tmp10, test_txt))
                remove(tmp10)
            else:
                self.assertTrue(False)
            if path.isfile(sjo):
                remove(sjo)
            if path.isfile(sjout):
                remove(sjout)
            if path.isfile(sjs):
                remove(sjs)
        self.teardown()

if __name__ == '__main__':
    unittest.main()