import subprocess
def run_echo(input_data):
    process = subprocess.Popen(
        ['./cmake-build-debug/paradigms_assignment_5'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    stdout, stderr = process.communicate(input_data)
    return stdout.strip(), stderr.strip()

def test_echo():
    tests = [
        {
            "input": "5 + 3 * 2\n",
            "expected_output": "Echo: 11"
        },
        {
            "input": "max(5, 2)\n",
            "expected_output": "Echo: 5"
        },
        {
            "input": "min(3, 4)\n",
            "expected_output": "Echo: 3"
        },
        {
            "input": "max(min(3 * 2, 2), 2)\n",
            "expected_output": "Echo: 2"
        },
        {
            "input": "var a = max(min(3 * 2, 2), 2)\na + 3\n",
            "expected_output": "Echo: 5"
        },
        {
            "input": "def myfunc(a, b) { min(a, b) + max(a, b) }\nmyfunc(3, 4)\n",
            "expected_output": "Echo: 7"
        }
    ]
    for test in tests:
        input_data = test["input"]
        expected_output = test["expected_output"]
        output, error = run_echo(input_data)
        if error:
            print(f"Test failed for input:\n{input_data}")
            print(f"Error:\n{error}")
        elif output.strip() != expected_output:
            print(f"Test failed for input:\n{input_data}")
            print(f"Expected:\n{expected_output}")
            print(f"Got:\n{output}")
        else:
            print(f"Test passed for input:\n{input_data}")

if __name__ == "__main__":
    test_echo()