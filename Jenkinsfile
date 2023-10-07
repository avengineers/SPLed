properties([
    buildDiscarder(logRotator(numToKeepStr: '20')),
    disableConcurrentBuilds()
])

node('SD-RM') {
    ws('sple/spled') {
        // git should use the Windows Store (certificates), but this fails sometimes
        bat 'git config --global http.sslVerify false'
        checkout scm

        dir('test/output') {
            deleteDir() // cleanup old test reports
        }

        bat '''
call build.bat -install || exit /b 1
call build.bat -target selftests || exit /b 0
'''
        junit allowEmptyResults: false, keepLongStdio: false, testResults: 'test/output/test-report.xml,build/**/test/src/**/junit.xml'

        dir('build') {
            archiveArtifacts(
                artifacts: '**/prod/spled.exe'
            )
        }
    }
}
