"""Test convenience variables when you drop in from lldb prompt into an embedded interpreter."""

import os
import unittest2
import lldb
from lldbtest import *

class ConvenienceVariablesCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    @skipUnlessDarwin
    @dsym_test
    @skipIfRemote
    def test_with_dsym_and_run_command(self):
        """Test convenience variables lldb.debugger, lldb.target, lldb.process, lldb.thread, and lldb.frame."""
        self.buildDsym()
        self.convenience_variables()

    @dwarf_test
    @skipIfFreeBSD # llvm.org/pr17228
    @skipIfRemote
    @expectedFailureAll("llvm.org/pr23560", oslist=["linux"], compiler="gcc", compiler_version=[">=","4.9"], archs=["i386"])
    @expectedFailureWindows("llvm.org/pr22274: need a pexpect replacement for windows")
    def test_with_dwarf_and_run_commands(self):
        """Test convenience variables lldb.debugger, lldb.target, lldb.process, lldb.thread, and lldb.frame."""
        self.buildDwarf()
        self.convenience_variables()

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # Find the line number to break on inside main.cpp.
        self.line = line_number('main.c', 'Hello world.')

    def convenience_variables(self):
        """Test convenience variables lldb.debugger, lldb.target, lldb.process, lldb.thread, and lldb.frame."""
        import pexpect
        exe = os.path.join(os.getcwd(), "a.out")
        prompt = "(lldb) "
        python_prompt = ">>> "

        # So that the child gets torn down after the test.
        self.child = pexpect.spawn('%s %s %s' % (lldbtest_config.lldbExec, self.lldbOption, exe))
        child = self.child
        # Turn on logging for what the child sends back.
        if self.TraceOn():
            child.logfile_read = sys.stdout

        # Set the breakpoint, run the inferior, when it breaks, issue print on
        # the various convenience variables.
        child.expect_exact(prompt)
        child.sendline('breakpoint set -f main.c -l %d' % self.line)
        child.expect_exact(prompt)
        child.sendline('run')
        child.expect_exact("stop reason = breakpoint 1.1")
        child.expect_exact(prompt)
        child.sendline('script')
        child.expect_exact(python_prompt)

        # Set a flag so that we know during teardown time, we need to exit the
        # Python interpreter, then the lldb interpreter.
        self.child_in_script_interpreter = True

        child.sendline('print lldb.debugger')
        child.expect_exact(python_prompt)
        self.expect(child.before, exe=False,
            patterns = ['Debugger \(instance: .*, id: \d\)'])

        child.sendline('print lldb.target')
        child.expect_exact(python_prompt)
        self.expect(child.before, exe=False,
            substrs = ['a.out'])

        child.sendline('print lldb.process')
        child.expect_exact(python_prompt)
        self.expect(child.before, exe=False,
            patterns = ['SBProcess: pid = \d+, state = stopped, threads = \d, executable = a.out'])

        child.sendline('print lldb.thread')
        child.expect_exact(python_prompt)
        # Linux outputs decimal tid and 'name' instead of 'queue'
        self.expect(child.before, exe=False,
            patterns = ['thread #1: tid = (0x[0-9a-f]+|[0-9]+), 0x[0-9a-f]+ a\.out`main\(argc=1, argv=0x[0-9a-f]+\) \+ \d+ at main\.c:%d, (name|queue) = \'.+\', stop reason = breakpoint 1\.1' % self.line])

        child.sendline('print lldb.frame')
        child.expect_exact(python_prompt)
        self.expect(child.before, exe=False,
            patterns = ['frame #0: 0x[0-9a-f]+ a\.out`main\(argc=1, argv=0x[0-9a-f]+\) \+ \d+ at main\.c:%d' % self.line])


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
