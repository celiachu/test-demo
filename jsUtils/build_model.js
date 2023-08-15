const config = require('../config/Config')
const {execSync, exec} = require('child_process')
const path = require('path')
const mkdirp = require('mkdirp')
const fs = require('fs')

const args = {}
process.argv.slice(2).forEach((arg) => {
  const [name, value] = arg.split('=')
  if (!value) throw new Error(`Invalid argument: ${name}`)
  args[name] = value
})

const isDevelop = args['develop'] === 'true'

const homeDir = path.join(__dirname, '..')
const clonedProtoDir = path.join(homeDir, '..', 'bingo-client-proto')
const workingProtoDir = path.join(homeDir, '..', 'bingo-client-proto')
const modelDir = path.join(homeDir, 'tmp', 'model')

if (!config || !config.protoConfig || !config.protoConfig.repo) {
  return console.log('请在config文件中添加{... protoConfig: {repo: YOUR_URL} ...}相关配置以指定协议仓库地址')
}

function pullRepo() {
  if (!fs.existsSync(clonedProtoDir)) {
    cloneRepo()
  }
  console.log(`git pull`)
  execSync(`git pull`, {cwd: clonedProtoDir})
}

function cloneRepo() {
  fs.rmdirSync(clonedProtoDir, {recursive: true})
  mkdirp.sync(clonedProtoDir)
  execSync(`git clone ${config.protoConfig.repo} ${clonedProtoDir}`, {cwd: clonedProtoDir})
}

async function buildModel(develop = false) {
  mkdirp.sync(modelDir)
  const protoDir = develop ? workingProtoDir : clonedProtoDir
  const docNames = fs.readdirSync(protoDir)
  let filenames = []
  docNames.forEach((docName) => {
    const doc = path.join(protoDir, docName)
    if (fs.statSync(doc).isDirectory()) {
      filenames.push(...fs.readdirSync(doc).map((_) => path.join(docName, _)))
    } else {
      filenames.push(docName)
    }
  })
  filenames = filenames.filter((filename) => filename.endsWith('.proto')).map((filename) => path.join(protoDir, filename))
  const modelFile = path.join(modelDir, 'model.js')
  const modelTsFile = path.join(modelDir, `model.d.ts`)

  console.log(`./node_modules/.bin/pbjs -t static-module -w commonjs -o ${modelFile} ${filenames.join(' ')}`)
  execSync(`./node_modules/.bin/pbjs -t static-module -w commonjs -o ${modelFile} ${filenames.join(' ')}`, {cwd: homeDir})
  console.log(`./node_modules/.bin/pbts -o ${modelTsFile} ${modelFile}`)
  execSync(`./node_modules/.bin/pbts -o ${modelTsFile} ${modelFile}`, {cwd: homeDir})

  const ModelMapping = {}
  const modelContent = require(modelFile)
  const packagesName = Object.keys(modelContent)
  packagesName.forEach((packageName) => {
    const modelNames = Object.keys(modelContent[packageName])
    modelNames.forEach((modelName) => {
      if (modelName.endsWith('_Req') || modelName.endsWith('_Ret')) {
        const cmd = modelName.slice(0, modelName.length - 4)
        const method = modelName.slice(modelName.length - 3, modelName.length)
        ModelMapping[cmd] = ModelMapping[cmd] || {}
        ModelMapping[cmd][method] = {
          packageName,
          modelName
        }
      }
    })
  })
  fs.writeFileSync(path.join(modelDir, 'overview.json'), JSON.stringify(ModelMapping))
}

if (!isDevelop) {
  pullRepo()
}
buildModel(isDevelop)
