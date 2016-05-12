`virtualenv venv`  
`source ./venv/bin/activate`  
`pip3 freeze > requirements.txt`

Insert the following lines into the breakpoint
```
from CallGraph import CallGraph
CallGraph()
```

Then run the python program to generate the call graph