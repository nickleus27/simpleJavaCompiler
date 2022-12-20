import unittest
from subprocess import run
from os import remove
from os import path
import filecmp

class Test_sJava_Files(unittest.TestCase):
    sjc_exe = "src/build/./sjc"
    exe = "."
    dir = "tests"

    def test_test0(self):
        test0 = path.join(self.dir, "test0.sjava")
        sjs = path.join(self.dir, "test0.sjava.s")
        sjo = path.join(self.dir, "test0.sjava.o")
        sjout = path.join(self.dir, "test0.sjava.out")
        tmp0 = path.join(self.dir, "temp0.txt")
        test_txt = path.join(self.dir, "test0.txt")
        exitStatus = run([self.sjc_exe, test0]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
            '''   
            with open(path.join(self.dir, "temp0.txt"), 'w') as outfile:
                run(path.join(self.exe, "test0.sjava.out"), stdout=outfile)
                outfile.close()
        if path.isfile(path.join(self.dir, "temp0.txt")):
            self.assertTrue(filecmp(path.join(self.dir, "temp0.txt"), path.join(self.dir, "test0.txt")))
        else:
            self.assertTrue(False)
            '''
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test1(self):
        test1 = path.join(self.dir, "test1.sjava")
        sjs = path.join(self.dir, "test1.sjava.s")
        sjo = path.join(self.dir, "test1.sjava.o")
        sjout = path.join(self.dir, "test1.sjava.out")
        tmp1 = path.join(self.dir, "temp1.txt")
        test_txt = path.join(self.dir, "test1.txt")
        exitStatus = run([self.sjc_exe, test1]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test2(self):
        test2 = path.join(self.dir, "test2.sjava")
        sjs = path.join(self.dir, "test2.sjava.s")
        sjo = path.join(self.dir, "test2.sjava.o")
        sjout = path.join(self.dir, "test2.sjava.out")
        tmp2 = path.join(self.dir, "temp2.txt")
        test_txt = path.join(self.dir, "test2.txt")
        exitStatus = run([self.sjc_exe, test2]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test2_1(self):
        test2_1 = path.join(self.dir, "test2-1.sjava")
        sjs = path.join(self.dir, "test2-1.sjava.s")
        sjo = path.join(self.dir, "test2-1.sjava.o")
        sjout = path.join(self.dir, "test2-1.sjava.out")
        tmp2_1 = path.join(self.dir, "temp2-1.txt")
        test_txt = path.join(self.dir, "test2-1.txt")
        exitStatus = run([self.sjc_exe, test2_1]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test3(self):
        test3 = path.join(self.dir, "test3.sjava")
        sjs = path.join(self.dir, "test3.sjava.s")
        sjo = path.join(self.dir, "test3.sjava.o")
        sjout = path.join(self.dir, "test3.sjava.out")
        tmp3 = path.join(self.dir, "temp3.txt")
        test_txt = path.join(self.dir, "test3.txt")
        exitStatus = run([self.sjc_exe, test3]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test4(self):
        test4 = path.join(self.dir, "test4.sjava")
        sjs = path.join(self.dir, "test4.sjava.s")
        sjo = path.join(self.dir, "test4.sjava.o")
        sjout = path.join(self.dir, "test4.sjava.out")
        tmp4 = path.join(self.dir, "temp4.txt")
        test_txt = path.join(self.dir, "test4.txt")
        exitStatus = run([self.sjc_exe, test4]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test4_1(self):
        test4_1 = path.join(self.dir, "test4-1.sjava")
        sjs = path.join(self.dir, "test4-1.sjava.s")
        sjo = path.join(self.dir, "test4-1.sjava.o")
        sjout = path.join(self.dir, "test4-1.sjava.out")
        tmp4_1 = path.join(self.dir, "temp4-1.txt")
        test_txt = path.join(self.dir, "test4-1.txt")
        exitStatus = run([self.sjc_exe, test4_1]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test5(self):
        test5 = path.join(self.dir, "test5.sjava")
        sjs = path.join(self.dir, "test5.sjava.s")
        sjo = path.join(self.dir, "test5.sjava.o")
        sjout = path.join(self.dir, "test5.sjava.out")
        tmp5 = path.join(self.dir, "temp5.txt")
        test_txt = path.join(self.dir, "test5.txt")
        exitStatus = run([self.sjc_exe, test5]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test6(self):
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
                with open(test_txt, "w") as outfile:
                    run(path.join(self.exe, sjout), stdout=outfile)
                    outfile.close()
            if path.isfile(sjo):
                remove(sjo)
            if path.isfile(sjout):
                remove(sjout)
            if path.isfile(sjs):
                remove(sjs)

    def test_test7(self):
        test7 = path.join(self.dir, "test7.sjava")
        sjs = path.join(self.dir, "test7.sjava.s")
        sjo = path.join(self.dir, "test7.sjava.o")
        sjout = path.join(self.dir, "test7.sjava.out")
        tmp7 = path.join(self.dir, "temp7.txt")
        test_txt = path.join(self.dir, "test7.txt")
        exitStatus = run([self.sjc_exe, test7]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(sjout):
            with open(test_txt, "w") as outfile:
                run(path.join(self.exe, sjout), stdout=outfile)
                outfile.close()
        if path.isfile(sjo):
            remove(sjo)
        if path.isfile(sjout):
            remove(sjout)
        if path.isfile(sjs):
            remove(sjs)

    def test_test8(self):
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
                with open(test_txt, "w") as outfile:
                    run(path.join(self.exe, sjout), stdout=outfile)
                    outfile.close()
            if path.isfile(sjo):
                remove(sjo)
            if path.isfile(sjout):
                remove(sjout)
            if path.isfile(sjs):
                remove(sjs)

if __name__ == '__main__':
    unittest.main()