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
        exitStatus = run([self.sjc_exe, path.join(self.dir,"test0.sjava")]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile(path.join(self.dir, "test0.sjava.out")):
            with open(path.join(self.dir,'test0.txt'), "w") as outfile:
                file_path = path.join(self.dir, "test0.sjava.out")
                run(path.join(self.exe, file_path), stdout=outfile)
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
        if path.isfile(path.join(self.dir, "test0.sjava.o")):
            remove(path.join(self.dir, "test0.sjava.o"))
        if path.isfile(path.join(self.dir, "test0.sjava.out")):
            remove(path.join(self.dir, "test0.sjava.out"))
        if path.isfile(path.join(self.dir, "test0.sjava.s")):
            remove(path.join(self.dir, "test0.sjava.s"))
            
    def test_test1(self):
        exitStatus = run([self.sjc_exe, "test1.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test1.sjava.out"):
            with open('test1.txt', "w") as outfile:
                run("./test1.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test1.sjava.o"):
            remove("test1.sjava.o")
        if path.isfile("test1.sjava.out"):
            remove("test1.sjava.out")
        if path.isfile("test1.sjava.s"):
            remove("test1.sjava.s")

    def test_test2(self):
        exitStatus = run([self.sjc_exe, "test2.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test2.sjava.out"):
            with open('test2.txt', "w") as outfile:
                run("./test2.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test2.sjava.o"):
            remove("test2.sjava.o")
        if path.isfile("test2.sjava.out"):
            remove("test2.sjava.out")
        if path.isfile("test2.sjava.s"):
            remove("test2.sjava.s")

    def test_test2_1(self):
        exitStatus = run([self.sjc_exe, "test2-1.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test2-1.sjava.out"):
            with open('test2-1.txt', "w") as outfile:
                run("./test2-1.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test2-1.sjava.o"):
            remove("test2-1.sjava.o")
        if path.isfile("test2-1.sjava.out"):
            remove("test2-1.sjava.out")
        if path.isfile("test2-1.sjava.s"):
            remove("test2-1.sjava.s")

    def test_test3(self):
        exitStatus = run([self.sjc_exe, "test3.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test3.sjava.out"):
            with open('test3.txt', "w") as outfile:
                run("./test3.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test3.sjava.o"):
            remove("test3.sjava.o")
        if path.isfile("test3.sjava.out"):
            remove("test3.sjava.out")
        if path.isfile("test3.sjava.s"):
            remove("test3.sjava.s")

    def test_test4(self):
        exitStatus = run([self.sjc_exe, "test4.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test4.sjava.out"):
            with open('test4.txt', "w") as outfile:
                run("./test4.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test4.sjava.o"):
            remove("test4.sjava.o")
        if path.isfile("test4.sjava.out"):
            remove("test4.sjava.out")
        if path.isfile("test4.sjava.s"):
            remove("test4.sjava.s")

    def test_test4_1(self):
        exitStatus = run([self.sjc_exe, "test4-1.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test4-1.sjava.out"):
            with open('test4-1.txt', "w") as outfile:
                run("./test4-1.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test4-1.sjava.o"):
            remove("test4-1.sjava.o")
        if path.isfile("test4-1.sjava.out"):
            remove("test4-1.sjava.out")
        if path.isfile("test4-1.sjava.s"):
            remove("test4-1.sjava.s")

    def test_test5(self):
        exitStatus = run([self.sjc_exe, "test5.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test5.sjava.out"):
            with open('test5.txt', "w") as outfile:
                run("./test5.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test5.sjava.o"):
            remove("test5.sjava.o")
        if path.isfile("test5.sjava.out"):
            remove("test5.sjava.out")
        if path.isfile("test5.sjava.s"):
            remove("test5.sjava.s")

    def test_test6(self):
        if path.isfile("test6.sjava"):
            exitStatus = run([self.sjc_exe, "test6.sjava"]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile("test6.sjava.out"):
                with open('test6.txt', "w") as outfile:
                    run("./test6.sjava.out", stdout=outfile)
                    outfile.close()
            if path.isfile("test6.sjava.o"):
                remove("test6.sjava.o")
            if path.isfile("test6.sjava.out"):
                remove("test6.sjava.out")
            if path.isfile("test6.sjava.s"):
                remove("test6.sjava.s")

    def test_test7(self):
        exitStatus = run([self.sjc_exe, "test7.sjava"]).returncode
        self.assertEqual(exitStatus, 0)
        if path.isfile("test7.sjava.out"):
            with open('test7.txt', "w") as outfile:
                run("./test7.sjava.out", stdout=outfile)
                outfile.close()
        if path.isfile("test7.sjava.o"):
            remove("test7.sjava.o")
        if path.isfile("test7.sjava.out"):
            remove("test7.sjava.out")
        if path.isfile("test7.sjava.s"):
            remove("test7.sjava.s")

    def test_test8(self):
        if path.isfile("test8.sjava"):
            exitStatus = run([self.sjc_exe, "test8.sjava"]).returncode
            self.assertEqual(exitStatus, 0)
            if path.isfile("test8.sjava.out"):
                with open('test8.txt', "w") as outfile:
                    run("./test8.sjava.out", stdout=outfile)
                    outfile.close()
            if path.isfile("test8.sjava.o"):
                remove("test8.sjava.o")
            if path.isfile("test8.sjava.out"):
                remove("test8.sjava.out")
            if path.isfile("test8.sjava.s"):
                remove("test8.sjava.s")

if __name__ == '__main__':
    unittest.main()