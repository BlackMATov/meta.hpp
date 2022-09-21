#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys

#
#
#

EMPTY_MATCHER = re.compile(r'^\s*$')
C_COMMENT_MATCHER = re.compile(r'^/\*.*\*/', re.S)

USER_INCLUDE_MATCHER = re.compile(r'#\s*include\s*\"(.*)\"')
SYSTEM_INCLUDE_MATCHER = re.compile(r'#\s*include\s*<(.*)>')
PRAGMA_ONCE_MATCHER = re.compile(r'#\s*pragma\s+once')

#
#
#

def CollectLicenseComment(headerPath):
    with open(headerPath, "r") as headerStream:
        headerContent = headerStream.read().strip()
        commentMatch = re.match(C_COMMENT_MATCHER, headerContent)
        return commentMatch.group() if commentMatch else ""

def CollectSystemIncludes(headerPath, parsedHeaders = set()):
    with open(headerPath, "r") as headerStream:
        headerContent = headerStream.read().strip()

        if PRAGMA_ONCE_MATCHER.search(headerContent) and headerPath in parsedHeaders:
            return set()

        headerIncludes = set()
        
        parsedHeaders.add(headerPath)
        headerLines = headerContent.split('\n')

        for headerLine in headerLines:
            includeMatch = USER_INCLUDE_MATCHER.findall(headerLine)
            if includeMatch:
                internalHeaderPath = os.path.abspath(os.path.join(os.path.dirname(headerPath), includeMatch[0]))
                headerIncludes = headerIncludes.union(CollectSystemIncludes(internalHeaderPath, parsedHeaders))
            includeMatch = SYSTEM_INCLUDE_MATCHER.findall(headerLine)
            if includeMatch:
                headerIncludes.add(includeMatch[0])

        return headerIncludes

def ParseHeader(headerPath, parsedHeaders = set()):
    with open(headerPath, "r") as headerStream:
        headerContent = headerStream.read().strip()
        headerContent = re.sub(C_COMMENT_MATCHER, '', headerContent)

        if PRAGMA_ONCE_MATCHER.search(headerContent) and headerPath in parsedHeaders:
            return ""
        
        parsedHeaders.add(headerPath)
        headerLines = headerContent.split('\n')

        outputContent = ""
        shouldSkipNextEmptyLines = True

        for headerLine in headerLines:
            if EMPTY_MATCHER.match(headerLine) and shouldSkipNextEmptyLines:
                continue

            if PRAGMA_ONCE_MATCHER.match(headerLine):
                shouldSkipNextEmptyLines = True
                continue
        
            includeMatch = USER_INCLUDE_MATCHER.findall(headerLine)
            if includeMatch:
                internalHeaderPath = os.path.abspath(os.path.join(os.path.dirname(headerPath), includeMatch[0]))
                internalHeaderContent = ParseHeader(internalHeaderPath, parsedHeaders)

                outputContent += internalHeaderContent
                shouldSkipNextEmptyLines = True
                continue
            
            includeMatch = SYSTEM_INCLUDE_MATCHER.findall(headerLine)
            if includeMatch:
                shouldSkipNextEmptyLines = True
                continue

            shouldSkipNextEmptyLines = False
            outputContent += "{}\n".format(headerLine)

        return "{}\n".format(outputContent)

#
#
#

inputHeaderPath = os.path.abspath(sys.argv[1])
outputHeaderPath = os.path.abspath(sys.argv[2])

os.makedirs(os.path.dirname(outputHeaderPath), exist_ok=True)

with open(outputHeaderPath, "w") as outputHeaderStream:
    licenseComment = CollectLicenseComment(inputHeaderPath)
    systemIncludes = CollectSystemIncludes(inputHeaderPath)

    outputHeaderStream.write("{}\n".format(licenseComment))
    outputHeaderStream.write("\n")
    
    for systemInclude in sorted(systemIncludes):
        outputHeaderStream.write("#include <{}>\n".format(systemInclude))
    outputHeaderStream.write("\n")

    outputHeaderStream.write(ParseHeader(inputHeaderPath).strip())
    outputHeaderStream.write("\n")
